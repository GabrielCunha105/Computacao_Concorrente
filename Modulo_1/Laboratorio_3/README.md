# Ganho de desempenho

## Vetor com 10⁵ elementos
### 2 threads

<table>
  <tr>
    <th></th>
    <th>Execução 1</th>
    <th>Execução 2</th>
    <th>Execução 3</th>
    <th>Execução 4</th>
    <th>Execução 5</th>
    <th>Média</th>
  </tr>
  <tr>
    <th>Sequencial</th>
    <td>0.000498s</td>
    <td>0.000394s</td>
    <td>0.000438s</td>
    <td>0.000264s</td>
    <td>0.000241s</td>
    <td>0.000367s</td>
  </tr>
  <tr>
    <th>Concorrente</th>
    <td>0.000424s</td>
    <td>0.000297s</td>
    <td>0.000502s</td>
    <td>0.000288s</td>
    <td>0.000254s</td>
    <td>0.000353s</td>
  </tr>
</table>
<strong>Aceleração: 1.04</strong><br>
<br>

### 4 threads

<table>
  <tr>
    <th></th>
    <th>Execução 1</th>
    <th>Execução 2</th>
    <th>Execução 3</th>
    <th>Execução 4</th>
    <th>Execução 5</th>
    <th>Média</th>
  </tr>
  <tr>
    <th>Sequencial</th>
    <td>0.000700s</td>
    <td>0.001122s</td>
    <td>0.000728s</td>
    <td>0.000784s</td>
    <td>0.000856s</td>
    <td>0.000838s</td>
  </tr>
  <tr>
    <th>Concorrente</th>
    <td>0.001001s</td>
    <td>0.000889s</td>
    <td>0.000576s</td>
    <td>0.000455s</td>
    <td>0.000543s</td>
    <td>0.000693s</td>
  </tr>
</table>
<strong>Aceleração: 1.21</strong><br>
<br>

## Vetor com 10⁷ elementos
### 2 threads

<table>
  <tr>
    <th></th>
    <th>Execução 1</th>
    <th>Execução 2</th>
    <th>Execução 3</th>
    <th>Execução 4</th>
    <th>Execução 5</th>
    <th>Média</th>
  </tr>
  <tr>
    <th>Sequencial</th>
    <td>0.026892s</td>
    <td>0.028547s</td>
    <td>0.026525s</td>
    <td>0.026160s</td>
    <td>0.025767s</td>
    <td>0.026778s</td>
  </tr>
  <tr>
    <th>Concorrente</th>
    <td>0.015014s</td>
    <td>0.016208s</td>
    <td>0.015746s</td>
    <td>0.014387s</td>
    <td>0.015203s</td>
    <td>0.015312s</td>
  </tr>
</table>
<strong>Aceleração: 1.75</strong><br>
<br>

### 4 threads

<table>
  <tr>
    <th></th>
    <th>Execução 1</th>
    <th>Execução 2</th>
    <th>Execução 3</th>
    <th>Execução 4</th>
    <th>Execução 5</th>
    <th>Média</th>
  </tr>
  <tr>
    <th>Sequencial</th>
    <td>0.025938s</td>
    <td>0.025162s</td>
    <td>0.026043s</td>
    <td>0.025334s</td>
    <td>0.026146s</td>
    <td>0.025725s</td>
  </tr>
  <tr>
    <th>Concorrente</th>
    <td>0.007988s</td>
    <td>0.009413s</td>
    <td>0.009298s</td>
    <td>0.007467s</td>
    <td>0.012040s</td>
    <td>0.009241s</td>
  </tr>
</table>
<strong>Aceleração: 2.78</strong><br>
<br>

## Vetor com 10⁹ elementos
### 2 threads

<table>
  <tr>
    <th></th>
    <th>Execução 1</th>
    <th>Execução 2</th>
    <th>Execução 3</th>
    <th>Execução 4</th>
    <th>Execução 5</th>
    <th>Média</th>
  </tr>
  <tr>
    <th>Sequencial</th>
    <td>2.524406s</td>
    <td>2.519930s</td>
    <td>2.518329s</td>
    <td>2.512281s</td>
    <td>2.488918s</td>
    <td>2.512773s</td>
  </tr>
  <tr>
    <th>Concorrente</th>
    <td>1.460114s</td>
    <td>1.448006s</td>
    <td>1.447548s</td>
    <td>1.440531s</td>
    <td>1.443032s</td>
    <td>1.447846s</td>
  </tr>
</table>
<strong>Aceleração: 1.74</strong><br>
<br>

### 4 threads

<table>
  <tr>
    <th></th>
    <th>Execução 1</th>
    <th>Execução 2</th>
    <th>Execução 3</th>
    <th>Execução 4</th>
    <th>Execução 5</th>
    <th>Média</th>
  </tr>
  <tr>
    <th>Sequencial</th>
    <td>2.496754s</td>
    <td>2.508326s</td>
    <td>2.504807s</td>
    <td>2.495933s</td>
    <td>2.496025s</td>
    <td>2.500369s</td>
  </tr>
  <tr>
    <th>Concorrente</th>
    <td>0.916397s</td>
    <td>0.889726s</td>
    <td>0.858977s</td>
    <td>0.901456s</td>
    <td>0.899427s</td>
    <td>0.893197s</td>
  </tr>
</table>
<strong>Aceleração: 2.80</strong><br>
<br>

