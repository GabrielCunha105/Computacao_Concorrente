import java.io.*;
import java.util.concurrent.Semaphore;

/**
 * Controlador do padrão Leitores/Escritores 
 */
class ControladorPC {
    private int[][] buffer;
    private int tamanhoBuffer;
    private int inicio;
    private int fim;
    private Semaphore vazio;
    private Semaphore cheio;

    /** 
     * Limite de quantos blocos serão consumidos, quando igual a 0, chamadas
     * subsequentes à função {@code consumir} retornarão {@value null}. 
     */
    private int limite;

    ControladorPC(int tamanhoBuffer) {
        this.tamanhoBuffer = tamanhoBuffer;
        inicio = 0;
        fim = 0;
        buffer = new int[tamanhoBuffer][];
        vazio = new Semaphore(tamanhoBuffer);
        cheio = new Semaphore(0);
        limite = -1;
    }

    /**
     * Define o limite de quantos blocos serão consumidos, quando a função {@code consumir},
     * for chamada {@code limite} vezes, chamadas subsequentes retornarão {@literal null}.
     * 
     * @param limite Novo limite
     */
    public synchronized void setLimite(int limite) {
        this.limite = limite;

        // Acorda as threads que estão esperando o limite ser definido
        this.notifyAll();
    }

    /**
     * Consome um bloco do buffer. Caso o {@code limite} ainda não tenha sido
     * definido, entra em espera.
     * 
     * @return Um bloco do buffer
     */
    public synchronized int[] consumir() {
        if (limite == -1) {
            try {
                //espera o limite ser definido
                this.wait();
            } catch (InterruptedException ignored) {};
        }

        if (limite == 0) return null;
        --limite;

        int[] bloco;
        try {
            cheio.acquire();
        } catch (InterruptedException ignored) {};
        bloco = buffer[inicio];
        inicio = (inicio + 1) % tamanhoBuffer;
        vazio.release();

        return bloco;
    }

    /**
     * Insere um novo bloco no buffer. Não é sincronizada pois há apenas
     * um produtor no programa.
     * 
     * @param bloco bloco a ser inserido no buffer
     */
    public void produzir(int[] bloco) {
        try {
            vazio.acquire();
        } catch (InterruptedException ignored) {};
        buffer[fim] = bloco;
        fim = (fim + 1) % tamanhoBuffer;
        cheio.release();

        return;
    }

}

/**
 * Thread produtora. Lê os blocos do arquivo de entrada.
 */
class Produtor extends Thread {
    private ControladorPC controlador;

    public Produtor(ControladorPC controlador) {
        this.controlador = controlador;
    }

    @Override
    public void run() {
        try {
            // Lê o total de valores no arquivo de entrada e define o limite
            // de blocos no controlador
            String linha = Main.inputReader.readLine();
            int numBlocos = Integer.parseInt(linha) / Main.N;
            controlador.setLimite(numBlocos);

            String valores[];
            int bloco[];

            // Lê os blocos e os insere no buffer
            while ((linha = Main.inputReader.readLine()) != null) {
			    valores = linha.split(" ");
                bloco = new int[Main.N];

                for (int j = 0; j < Main.N; ++j) {
                    bloco[j] = Integer.parseInt(valores[j]);
                }
                controlador.produzir(bloco);
            }
        } catch (Exception e) {
            System.out.println("Erro ao ler arquivo de entrada");
        };
        return;
    }
}

/**
 * Thread consumidora. Lê os blocos do buffer, ordena os seus valores, 
 * e os insere no arquivo de saída.
 */
class Consumidor extends Thread {
    private ControladorPC controlador;
    
    Consumidor(ControladorPC controlador) {
        this.controlador = controlador;
    }

    /**
     * Ordena um bloco em ordem crescente.
     * 
     * @param bloco bloco a ser ordenado
     */
    private void ordenarBloco(int[] bloco) {
        int temp;
        for (int i = 0; i < Main.N; ++i) {
            for (int j = 0; j < Main.N-1; ++j) {
                if (bloco[j] > bloco[j+1]) {
                    temp = bloco[j];
                    bloco[j] = bloco[j+1];
                    bloco[j+1] = temp;
                }
            }
        }
        return;
    }

    /**
     * Converte um bloco para {@code String}
     * 
     * @param bloco bloco a ser convertido
     * @return String contendo os valores do bloco fornecido
     */
    private String blocoToString(int[] bloco) {
        String valores[] = new String[Main.N];
        for (int i = 0; i < Main.N; ++i) {
            valores[i] = String.valueOf(bloco[i]);
        }
        return String.join(" ", valores);
    }

    @Override
    public void run() {
        int[] bloco;

        while ((bloco = controlador.consumir()) != null) {
            ordenarBloco(bloco);
            String out = blocoToString(bloco);

            // Escreve no arquivo de saída. Sincronização permite que apenas
            // uma thread escreva no arquivo de saída de cada vez
            synchronized (Main.outputWriter) {
                try {
                    Main.outputWriter.write(out);
                    Main.outputWriter.newLine();
                } catch (IOException e) {
                    System.out.println("Erro ao escrever no arquivo de saída");
                }
            }
        }
        return;
    }
}

public class Main {

    /** Número de threads consumidoras */ 
    public static int C;

    /** Tamanho de cada bloco */
    public static int N;

    /** Leitor do arquivo de entrada */
    public static BufferedReader inputReader;

    /** Escritor do arquivo de saída */
    public static BufferedWriter outputWriter;

    /** Controlador do padrão Leitores/Escritores */
    public static ControladorPC controlador;

    /**
     * Lê os argumentos passados pelo usuário e inicia as variáveis globais.
     * 
     * @param args Argumentos passados pelo usuário
     * 
     * @throws IllegalArgumentException Número de argumentos inválido
     * @throws NumberFormatException O valor de {@code C} ou de {@code N} fornecido não é um inteiro
     * @throws FileNotFoundException O Arquivo de entrada não foi encontrado
     * @throws IOException Erro ao criar arquivo de saída
     */
    public static void iniciarVars(String[] args)
    throws IllegalArgumentException, NumberFormatException, FileNotFoundException,
    IOException {
        if (args.length != 4) {
            throw new IllegalArgumentException(
                "Digite \"java Main <C> <N> <Arquivo de entrada> <Arquivo de saída>\""
            );
        }
        C = Integer.parseInt(args[0]);
        N = Integer.parseInt(args[1]);

        // Cria um controlador cujo buffer tem capacidade para 10 blocos
        controlador = new ControladorPC(10);

        FileReader fReader = new FileReader(args[2]);
        inputReader = new BufferedReader(fReader);
        FileWriter fWriter = new FileWriter(args[3]);
        outputWriter = new BufferedWriter(fWriter);

        return;
    }

    public static void main(String[] args) {
        long inicio = System.nanoTime();

        // Lê os argumentos do usuário e inicia as variaveis globais
        try {
            iniciarVars(args);
        } catch (Exception e) {
            System.out.println(e.getMessage());
            return;
        }

        long etapa1 = System.nanoTime();

        // Cria uma thread produtora e C threads consumidoras
        Thread threads[] = new Thread[C+1];
        threads[0] = new Produtor(controlador);
        for (int i = 1; i < C+1; ++i) {
            threads[i] = new Consumidor(controlador);
        }

        long etapa2 = System.nanoTime();

        // Inicia todas as threads
        for (Thread t : threads) {
            t.start();
        }

        // Espera todas as threads terminarem
        for (Thread t : threads) {
            try {
                t.join();
            } catch (InterruptedException ignored) {};
        }

        long etapa3 = System.nanoTime();

        // Encerra os fluxos de I/O
        try {
            inputReader.close();
            outputWriter.close();
        } catch (IOException ignored) {};

        long fim = System.nanoTime();

        // Imprime o tempo de cada etapa da execução
        System.out.println("Etapa\t\t\t\tTempo\t\tPorcentagem do tempo total de execução\n");
        System.out.printf("Iniciando variáveis globais\t%dns\t%.1f%%\n",etapa1 - inicio, 100.0d*(etapa1- inicio)/(fim-inicio));
        System.out.printf("Criando as Threads\t\t%dns\t%.1f%%\n", (etapa2 - etapa1), 100.0d*(etapa2 - etapa1)/(fim-inicio));
        System.out.printf("Execuntando as Threads\t\t%dns\t%.1f%%\n", (etapa3 - etapa2), 100.0d*(etapa3 - etapa2)/(fim-inicio));
        System.out.printf("Fechando Buffers de I/O\t\t%dns\t%.1f%%\n", (fim - etapa3), 100.0d*(fim - etapa3)/(fim-inicio));
        System.out.printf("Total do Programa\t\t%dns\t100%%\n", fim-inicio);

        return;
    }

}