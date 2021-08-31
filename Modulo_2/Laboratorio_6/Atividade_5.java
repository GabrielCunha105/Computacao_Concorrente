import java.util.Random;

class Contador {
    private int contagem;

    Contador() {
        contagem = 0;
    }

    public synchronized void incrementar(int n) {
        contagem += n;
    }

    public synchronized int get() {
        return contagem;
    }
}

class Tarefa extends Thread {
    private int id;
    private int vetor[];
    private Contador contador;

    Tarefa(int id, int vetor[], Contador contador) {
        this.id = id;
        this.vetor = vetor;
        this.contador = contador;
    }

    public void run() {
        System.out.println("Thread " + id + " iniciou a contagem");
        int numPares = 0;
        for (int i = id; i < vetor.length; i += Atividade_5.NTHREADS) {
            if (vetor[i] % 2 == 0) numPares++;
        }
        contador.incrementar(numPares);
        System.out.println("Thread " + id + " terminou");
    }
}

public class Atividade_5 {
    public static int NTHREADS;
    public static int VLENGTH;

    public static void ler_args(String[] args)
    throws IllegalArgumentException, NumberFormatException {
        if (args.length != 2) {
            throw new IllegalArgumentException("Digite \"java Atividade_5 <Comprimento do Vetor> <Número de Threads>\"");
        }
        VLENGTH = Integer.parseInt(args[0]);
        NTHREADS = Integer.parseInt(args[1]);

        if (NTHREADS > VLENGTH) NTHREADS = VLENGTH;

        return;
    }
    
    public static int popularVetor(int vetor[]) {
        int numPares = 0;
        Random random = new Random();
        for (int i = 0; i < vetor.length; i++) {
            vetor[i] = random.nextInt(1024);
            if (vetor[i] % 2 == 0) numPares++;
        }

        return numPares;
    }

    public static void main(String[] args) {
        try {
            ler_args(args);
        } catch (Exception e) {
            System.out.println("Erro: " + e.getMessage());
        }

        int vetor[] = new int[VLENGTH];
        int gabarito = popularVetor(vetor);
        Contador contador = new Contador();
        Tarefa threads[] = new Tarefa[NTHREADS];

        System.out.println("Calculando número de inteiros pares em um vetor de comprimento igual a "
            + VLENGTH + " com " + NTHREADS + " threads.");

        //cria as threads da aplicacao
        for (int i = 0; i < NTHREADS; i++) {
            threads[i] = new Tarefa(i, vetor, contador);
        }
        //inicia as threads
        for (int i=0; i<threads.length; i++) {
           threads[i].start();
        }
        //espera pelo termino de todas as threads
        for (int i=0; i<threads.length; i++) {
           try { threads[i].join(); } catch (InterruptedException e) { return; }
        }

        // Verifica se o resultado está correto e imprime ele na tela
        int resultado = contador.get();
        if (resultado == gabarito) {
            System.out.println("Contagem bem-sucedida.");
            System.out.println("Número de valores pares: " + resultado);
        } else {
            System.out.println("Erro: Resultado diverge do gabarito.");
        }
    }
}
