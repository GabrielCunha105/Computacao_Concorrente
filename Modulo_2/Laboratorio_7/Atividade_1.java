// Monitor que implementa a logica do padrao leitores/escritores
class Monitor {
    private int leit, escr;

    // Construtor
    Monitor() {
        this.leit = 0; // leitores lendo (0 ou mais)
        this.escr = 0; // escritor escrevendo (0 ou 1)
    }

    // Entrada para leitores
    public synchronized void EntraLeitor(int id) {
        try {
            while (this.escr > 0) {
                System.out.println("le.leitorBloqueado(" + id + ")");
                wait(); // bloqueia pela condicao logica da aplicacao
            }
            this.leit++; // registra que ha mais um leitor lendo
            System.out.println("le.leitorLendo(" + id + ")");
        } catch (InterruptedException e) {
        }
    }

    // Saida para leitores
    public synchronized void SaiLeitor(int id) {
        this.leit--; // registra que um leitor saiu
        if (this.leit == 0)
            this.notify(); // libera escritor (caso exista escritor bloqueado)
        System.out.println("le.leitorSaindo(" + id + ")");
    }

    // Entrada para escritores
    public synchronized void EntraEscritor(int id) {
        try {
            while ((this.leit > 0) || (this.escr > 0)) {
                System.out.println("le.escritorBloqueado(" + id + ")");
                wait(); // bloqueia pela condicao logica da aplicacao
            }
            this.escr++; // registra que ha um escritor escrevendo
            System.out.println("le.escritorEscrevendo(" + id + ")");
        } catch (InterruptedException e) {
        }
    }

    // Saida para escritores
    public synchronized void SaiEscritor(int id) {
        this.escr--; // registra que o escritor saiu
        notifyAll(); // libera leitores e escritores (caso existam leitores ou escritores bloqueados)
        System.out.println("le.escritorSaindo(" + id + ")");
    }
}


// Thread Leitora
class L extends Thread {
    private int id;
    private Monitor monitor;

    L(int id, Monitor monitor) {
        this.id = id;
        this.monitor = monitor;
    }

    private static boolean verificarPrimo() {
        if (Atividade_1.var < 1) return false;
        for (int i = 2; i <= Atividade_1.var / 2; i++) {
            if (Atividade_1.var % i == 0) return false;
        }
        return true;
    }

    @Override
    public void run() {
        monitor.EntraLeitor(id);
        if (verificarPrimo()) {
            System.out.println("Leitor " + id + ": Número " + Atividade_1.var + " é primo");
        } else {
            System.out.println("Leitor " + id + ": Número " + Atividade_1.var + " não é primo");
        }
        monitor.SaiLeitor(id);
    }
}

// Thread Escritora
class E extends Thread {
    private int id;
    private Monitor monitor;

    E(int id, Monitor monitor) {
        this.id = id;
        this.monitor = monitor;
    }

    @Override
    public void run() {
        monitor.EntraEscritor(id);
        Atividade_1.var = this.id;
        System.out.println("Escritor " + id + ": var = " + Atividade_1.var);
        monitor.SaiEscritor(id);
    }
}

// Thread Leitora/Escritora
class LE extends Thread {
    private int id;
    private Monitor monitor;

    LE(int id, Monitor monitor) {
        this.id = id;
        this.monitor = monitor;
    }

    @Override
    public void run() {
        monitor.EntraLeitor(id);
        if (Atividade_1.var % 2 == 0) {
            System.out.println("Leitor/Escritor " + id + ": Número " + Atividade_1.var + " é par");
        } else {
            System.out.println("Leitor/Escritor " + id + ": Número " + Atividade_1.var + " é ímpar");
        }
        monitor.SaiLeitor(id);
        monitor.EntraEscritor(id);
        Atividade_1.var *= 2;
        System.out.println("Leitor/Escritor " + id + ": var = " + Atividade_1.var);
        monitor.SaiEscritor(id);
    }
}

public class Atividade_1 {
    public static int NUM_E;
    public static int NUM_L;
    public static int NUM_LE;
    public static int var = 0;

    public static void ler_args(String[] args)
    throws IllegalArgumentException, NumberFormatException {
        if (args.length != 3) {
            throw new IllegalArgumentException("Digite \"java Atividade_1 <Número de Leitoras> <Número de Escritoras> <Número de Leitoras/Escritoras>\"");
        }
        NUM_L = Integer.parseInt(args[0]);
        NUM_E = Integer.parseInt(args[1]);
        NUM_LE = Integer.parseInt(args[2]);

        return;
    }

    public static void main(String[] args) {
        try {
            ler_args(args);
        } catch (Exception e) {
            System.out.println("Erro: " + e.getMessage());
        }

        Thread threads[] = new Thread[NUM_L + NUM_E + NUM_LE];
        Monitor monitor = new Monitor();

        // cria as threads da aplicacao
        for (int i = 0; i < threads.length;) {
            if (--NUM_E >= 0)
                threads[i++] = new E(i, monitor);
            if (--NUM_L >= 0)
                threads[i++] = new L(i, monitor);
            if (--NUM_LE >= 0)
                threads[i++] = new LE(i, monitor);
        }

        // inicia as threads
        for (int i = 0; i < threads.length; i++) {
            threads[i].start();
        }
        // espera pelo termino de todas as threads
        for (int i = 0; i < threads.length; i++) {
            try {
                threads[i].join();
            } catch (InterruptedException e) {
                return;
            }
        }
        return;
    }
}
