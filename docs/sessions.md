# TODO

## Identificación de Sesiones y Clientes

> Estado: Diseño

El identificador de una sesión se entregará cuando un estado establezca conexión al servicio de sesiones de otro estado.

Esto implica que cada sesión del estado será reconocible ante otros estados a través de diferentes identificadores.

En otras palabras;

> Estado `A` iniciará conexión con `B` y `C` recibiendo los identificadores `X` y `Y`. 
> 
> Obligando a `A` a transaccionar con `B` usando el identificador `X` e `Y` para transaccionar con `C`.
> 
> Sí `A` transacciona con `C` usando `X` entonces la transacción no deberá ser procesada.

Esto requiere una serie de modificaciones:

- Todas las solicitudes y controladores deberán prescindir del identificador de la sesión para transaccionar entre estados.
- Posterior al establecimiento de una sesión o cliente; El estado respectivo deberá generar y enviar el identificador respectivo. 
- Cuando un cliente establezca conexión con un estado en particular este último deberá informar a todos los otros estados.

Esto producirá las siguientes estructuras:

Estado `A`:

- Clientes:

| Cliente | ID                                   | IP            | Puerto Local | Puerto Remoto |
|---------|--------------------------------------|---------------|--------------|---------------|
| 1       | 6a4b2fbd-0222-47c2-aadc-7c7662117ea0 | 192.168.5.100 | 9000         | 49217         |
| 2       | edd5184b-8656-40fc-bee4-ecb493c631d6 | 192.168.5.101 | 9000         | 49784         |
| 3       | 11f6102f-546b-4740-bf90-ab973ba6a9ff | 192.168.5.102 | 9000         | 50391         |

- Sesiones:

| Sesión | ID                                   | IP             | Puerto Local | Puerto Remoto |
|--------|--------------------------------------|----------------|--------------|---------------|
| B      | 0f6b1908-a518-4d0d-95ee-efbbf90bb912 | 192.168.10.101 | 10000        | 51206         |
| C      | 68a278d5-6678-48d0-a4de-e747d1eb588d | 192.168.10.102 | 10000        | 51988         |
| D      | 7bfa2acd-7594-468e-8272-050557a1fa7a | 192.168.10.103 | 10000        | 59347         |

> Este Estado fue el primero en iniciar operaciones entonces todos los siguientes Estados se conectan a él.

---

Estado `B`:

- Clientes:

| Cliente | ID                                   | IP            | Puerto Local | Puerto Remoto |
|---------|--------------------------------------|---------------|--------------|---------------|
| 4       | b5102be2-3bce-4101-9711-ac05a1ab1c5e | 192.168.5.103 | 9000         | 52743         |

- Sesiones:

| Sesión | ID                                   | IP             | Puerto Local | Puerto Remoto |
|--------|--------------------------------------|----------------|--------------|---------------|
| A      | 001a684a-0389-4f35-8e86-3947b200bb98 | 192.168.10.100 | 51206        | 10000         |
| C      | 54c6dde5-a99a-43f9-892e-5b6e79c540ee | 192.168.10.102 | 10000        | 53419         |
| D      | abdfa78d-b403-44ae-8a7d-c64f4cd3ba2d | 192.168.10.103 | 10000        | 58612         |

> Este Estado fue el segundo en iniciar operaciones entonces se conectó a `A` y todos los demás se conectan a él.

---

Estado `C`:

- Clientes:

| Cliente | ID                                   | IP            | Puerto Local | Puerto Remoto |
|---------|--------------------------------------|---------------|--------------|---------------|
| 5       | cb92d185-ff0c-441b-a76c-bde85e3f8e5d | 192.168.5.104 | 9000         | 54960         |
| 6       | 0e486595-7b95-4511-8efc-0b9909d8bd48 | 192.168.5.105 | 9000         | 57104         |

- Sesiones:

| Sesión | ID                                   | IP             | Puerto Local | Puerto Remoto |
|--------|--------------------------------------|----------------|--------------|---------------|
| A      | a0db39dd-7203-456a-a6db-f462ed10cc42 | 192.168.10.100 | 51988        | 10000         |
| B      | c0519345-3b03-4f42-9eaa-126594766127 | 192.168.10.101 | 53419        | 10000         |
| D      | 7e23f45d-1189-4f94-aa3f-9568ce509a21 | 192.168.10.103 | 10000        | 57866         |

> Este Estado se conectó a `B` inicialmente y fue informado de la existencia de `A`
> 
> Finalmente, `D` inició conexión con este Estado.

---

Estado `D`:

- Clientes:

| Cliente | ID                                   | IP            | Puerto Local | Puerto Remoto |
|---------|--------------------------------------|---------------|--------------|---------------|
| 7       | abd1a93d-2452-4e5b-83ba-5d695e59521c | 192.168.5.106 | 9000         | 60189         |
| 8       | 165c8762-4dff-4e57-a9a2-0fbb32132ecc | 192.168.5.107 | 9000         | 60934         |

- Sesiones:

| Sesión | ID                                   | IP             | Puerto Local | Puerto Remoto |
|--------|--------------------------------------|----------------|--------------|---------------|
| A      | 6597adcd-4223-422d-bd84-cc09348af2e9 | 192.168.10.100 | 59347        | 10000         |
| B      | a0ddae90-63eb-4f94-b038-648e8692a0e0 | 192.168.10.101 | 58612        | 10000         |
| C      | 4a25b57a-95cc-4bdf-a89e-b5f9e9caab88 | 192.168.10.102 | 57866        | 10000         |

> Este Estado se conectó a `C` inicialmente y fue informado de la existencia `A` y `B`.