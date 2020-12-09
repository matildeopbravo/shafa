/**
 @file modulo_f.c
 Responsavel pela compressao RLE e pelo calculo das frequencias.
*/
//include "ModuloF.h"
#include "modulo_f.h"
#include "fsize.h"

/* Codigo ASCII */
#define uintArroba 64
#define uint_PontoVirgula 59

#define WriteFreq_ERROR_IN_FILE -1 /* ERROR: Opening file */
#define WriteFreq_ERROR_IN_FREQ -2 /* ERROR: Reading the frequency */

/* Open a file */
FILE *openFile( const char *file, const char *mode){
    FILE *new_file = NULL;
    new_file = fopen( file, mode);
    return new_file;
}

/* Close file */
void closeFile(FILE **file){
    if( !file || !(*file))
        return;

    fclose(*file);
    *file = NULL;
}

/* FREQUENCY */
/* allocar memoria para as frequencias */
void initializeFreq( FreqBlock *e ){
    e = (FreqBlock *)calloc(1,sizeof(FreqBlock));
    (*e)->prox = NULL;
}

/* FILE */

/* INICIALIZAR */
/* Alocar memoria pra os blocos */
void initializeBlockFiles( BlockFiles *e ){
    e = (BlockFiles *)calloc(1,sizeof(BlockFiles));
    (*e)->blocks = NULL;
    (*e)->compression_type = NOT_COMPRESSED;
    /* (*e)->num_blocks = 0; */
}

void initializeBlocks( Blocks *e ){
    e = (Blocks *)calloc(1,sizeof(Blocks));
    (*e)->prox = NULL;
    /* (*e)->block_size = 0; */
}

void initializeBlockList( BlockList *e ){
    e = (BlockList *)calloc(1,sizeof(BlockList));
    /* (*e)->value = 0; */
    (*e)->prox = NULL;
}

/* Adicionar "algo" */
/* Add um elemento a uma lista de blocos */
void add( BlockList *e, uint8_t value){
    BlockList *new, *aux;
    aux = e;
    initializeBlockList( new );
    (*new)->value = value;
    for( ; (*aux)->prox!=NULL; aux=&((*e)->prox) ) ;
    (*aux)->prox = *new;
}


/* Write the frequencies in a file */
int writeFreq( FILE *fp_in , unsigned char *filename, BlockFiles *BlockFile , FreqBlock *freq ){
    FILE *fp;
    long long n_blocks; n_blocks = (*BlockFile)->num_blocks ;
    int fseek_error;
    char compression_type;
    int i,j; i = 1;
    Blocks *aux_Blocks; aux_Blocks = &(*BlockFile)->blocks;
    FreqBlock *aux_Freq; aux_Freq = freq;
    unsigned int block_size;

    /* Check that the frequency data is not empty */
    if( freq==NULL ) return WriteFreq_ERROR_IN_FILE;

    /* Open the file in binary write mode */
    if ( filename==NULL || *filename == 0 ) fp = fp_in;
    else{
        fp = fopen(filename, "r"); /*  fp = fopen(filename, "rb");*/
        if( fp == NULL ) return WriteFreq_ERROR_IN_FILE;
    }

    if ((*BlockFile)->compression_type == NOT_COMPRESSED) compression_type = 'N';
    else compression_type = 'N';

    /* ESCREVER:  */
    /* dados inicias */
    /* @<R|N>@NumerodeBlocos */
    /* fprintf( fp , "%c%c%c%d",uintArroba,compression_type,uintArroba,n_blocks); */
    /* ou entao */

    /* @<R|N>@NumerodeBlocos */
    fprintf( fp , "@%c%@%d",compression_type,n_blocks);
    /* Escrever as frequencias de um bloco */
    while( /* aux_Blocks && */ aux_Freq  && i<=n_blocks ){
        /* block_size = (*aux_Blocks)->block_size; */
        fprintf( fp , "@" );
        j = 0;
        /* Escrever as frequencias ate ao simbolo 254 */
        while( j<255 ){
            fprintf( fp, "%d;", (*aux_Freq)->freq[j] );
            fprintf( fp, "%d;", (*aux_Freq)->freq[j+1] );
            fprintf( fp, "%d;", (*aux_Freq)->freq[j+2] );
            fprintf( fp, "%d;", (*aux_Freq)->freq[j+3] );
            fprintf( fp, "%d;", (*aux_Freq)->freq[j+4] );
            j = j + 5;
        }
        /* frequencia do simbolo 255 */
        fprintf( fp, "%d", (*aux_Freq)->freq[j] );

        i = i + 1;
        aux_Freq = &(*aux_Freq)->prox;
        /* aux_Blocks = &(*aux_Blocks)->prox; */
    }
    if ( aux_Freq == NULL && i != n_blocks) return WriteFreq_ERROR_IN_FREQ;
    fprintf( fp , "@");
    return 1;
}


/*  */

int ModuloF( unsigned char *filename, unsigned long *the_block_size,  enum compression compression){

    /* ler o ficheiro */
    /* sugestao: ao colocar o file por blocos fazer ja a contagem */

    /* fazer compressao ou nao */
    /* se hover compressao fazer contagem dos simbolos */

    /* Frequencias */
}
