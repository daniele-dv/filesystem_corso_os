# Progetto Sistemi Operativi - Filesystem in C

## Descrizione

Il progetto consiste nella realizzazione di una semplice shell in linguaggio C che permette di creare e gestire un filesystem persistente, utilizzando la tecnica del memory mapping (`mmap()`).

Il filesystem viene memorizzato all'interno di un file e organizzato in tre parti principali:

- **Superblocco:** contiene i metadati del filesystem, come la dimensione totale e la posizione delle diverse aree.
- **Tabella degli inode:** contiene le informazioni relative ai file e alle directory, come nome, tipo, inode padre e posizione dei dati.
- **Area dati:** contiene il contenuto effettivo dei file.

La shell permette di creare file e directory, navigare tra le directory, scrivere e leggere il contenuto dei file ed eliminare gli elementi presenti nel filesystem.

I dati vengono conservati anche dopo la chiusura del programma e possono essere recuperati riaprendo il filesystem precedentemente creato.

## Compilazione
Per compilare il programma basta posizionarsi nella directory principale del progetto ed eseguire:

```bash
make
```

Ottenendo così l'eseguibile `myfs`.

Nel caso si voglia eliminare l'eseguibile compilato:

```bash
make clean
```

## Avvio del programma

Per avviare la shell senza aprire un filesystem esistente basta eseguire :

```bash
./myfs
```

Una volta avviato il programma, è possibile creare un filesystem utilizzando il comando:

```text
format nome.fs 30000
```

Il comando crea un filesystem chiamato `nome.fs` con una dimensione totale di 30000 byte.

**Attenzione:** `format` inizializza il filesystem e sovrascrive l'eventuale contenuto precedente del file passato come parametro.

Se invece si vuole riaprire un filesystem già esistente senza perdere i dati salvati bisogna usare :

```bash
./myfs nome.fs
```

In questo caso, il programma recupera il filesystem precedentemente creato e permette di continuare a utilizzarlo.

## Comandi disponibili

| Comando | Descrizione |
|---|---|
| `format <nome_file> <size>` | Crea e inizializza un nuovo filesystem della dimensione specificata in byte. |
| `mkdir <nome_directory>` | Crea una directory all'interno della directory corrente. |
| `cd <nome_directory>` | Permette di entrare in una directory. |
| `cd ..` | Torna alla directory padre. |
| `cd /` | Torna alla directory root. |
| `ls` | Visualizza i file e le directory presenti nella directory corrente. |
| `touch <nome_file>` | Crea un file vuoto. |
| `append <nome_file> <testo>` | Aggiunge del testo alla fine del contenuto di un file. |
| `cat <nome_file>` | Visualizza il contenuto di un file. |
| `rm <nome_file/directory>` | Elimina un file oppure una directory vuota. |
| `close` | Sincronizza il filesystem, lo chiude e termina il programma. |

I comandi che modificano file e directory lavorano unicamente sugli elementi presenti nella directory corrente, non tenendo in considerazione evenutali omonimi presenti in altre cartelle.

## Esempio di utilizzo

Dopo aver avviato il programma con `./myfs`, è possibile creare un filesystem e utilizzarlo nel seguente modo:

```text
myfs> format prova.fs 30000

myfs:/> mkdir documenti
myfs:/> cd documenti

myfs:/documenti> touch appunti.txt
myfs:/documenti> append appunti.txt Ciao mondo

myfs:/documenti> cat appunti.txt
Ciao mondo

myfs:/documenti> ls
appunti.txt

myfs:/documenti> close
```

Il comando `close` salva le modifiche e termina il programma.

Per recuperare il filesystem nella successiva esecuzione, come detto in precedenza, basta usare :

```bash
./myfs prova.fs
```

A questo punto sarà possibile accedere nuovamente ai file creati:

```text
myfs:/> cd documenti

myfs:/documenti> cat appunti.txt
Ciao mondo
```

Questo dimostra che i dati vengono conservati tra esecuzioni diverse del programma.

## Struttura del progetto
I file sorgente sono organizzati nel seguente modo:

- **main.c:** contiene il ciclo principale della shell e gestisce l'eventuale apertura di un filesystem esistente all'avvio.
- **comandi.c / comandi.h:** gestiscono il riconoscimento dei comandi inseriti dall'utente.
- **filesystem.c / filesystem.h:** contengono le strutture dati e le funzioni che permettono di gestire il filesystem.

## Limitazioni

Questo tipo di filesystem ha alcune limitazioni :

- Il numero massimo di inode disponibili è fissato a 128.
- Il comando `rm` permette di eliminare le directory soltanto quando sono vuote.
- L'eliminazione di un file libera il relativo inode, ma non recupera lo spazio che il suo contenuto occupava nell'area dati.
- Quando `append` deve spostare il contenuto di un file in una nuova posizione, la vecchia area occupata non viene recuperata.