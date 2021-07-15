# Módulo 1 - Laboratório 1
Gabriel Rodrigues Cunha - 119143696<br>
<br>

## Atividade 1
Cada vez que o programa `hello` é executado, as threads executam em uma ordem diferente. Isso ocorre pois no programa não há nenhuma instrução de controle da execução das threads, sendo assim, toda vez que uma nova thread é criada, o sistema operacional é quem decide em qual ordem as threads serão executadas, e esta ordem pode mudar cada vez que o programa é executado.

## Atividade 2
A diferença do `hello_arg` em relação ao `hello` é que neste programa, cada uma das threads imprime o seu identificador local, além da mensagem *Hello World*.

## Atividade 3
Sim, o programa funcionou como esperado. Toda vez que uma thread foi criada, esta recebeu um argumento do tipo `t_Args` que continha o seu identificador local e o número total de threads geradas pelo programa. Essas informações foram, então, impressas pela função `PrintHello` quando cada thread foi executada.

## Atividade 4
No programa `hello_join`, a thread principal só termina após todas as outras threads já terem terminado a sua execução. Isso ocorre pois na thread principal desse programa, ao contrário dos demais, é chamada a função `pthread_join` para todas as outras threads criadas, ou seja, a thread principal espera todas as outras threads terminarem a sua execução antes de terminar a sua própria.