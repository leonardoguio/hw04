#include <stdio.h>
#include <string.h>

int count = 0;
int instructions;

#define COIL_SADDLES           300
#define SEQUENCES_QUANTITY     100
#define INSTRUCTIONS_QUANTITY   14

typedef struct
{
  int ins[INSTRUCTIONS_QUANTITY];
} Sequence;

Sequence sequences[SEQUENCES_QUANTITY];

int generated_sequences;

int SequenceComparator(const void *a, const void *b)
{
  int i;
  Sequence *sa;
  Sequence *sb;
  
  sa = (Sequence*)a;
  sb = (Sequence*)b;
  
  for(i=0; i<INSTRUCTIONS_QUANTITY; i++)
  {
    if((*sa).ins[i] != (*sb).ins[i])
      return((*sa).ins[i] - (*sb).ins[i]);
  }
  return 0;
}

int InstructionComparator(const void *a, const void *b)
{
  return ((*(int*)a) - (*(int*)b));
}

int IsThereEqualInstruction(Sequence *seq, int index)
{
  int i;
  for(i=0; i<index; i++)
  {
    if(seq->ins[i] == seq->ins[index])
    {
      return 1;
      printf("EQUAL INSTRUCTION!\n");
    }
  }
  return 0;
}

void GenerateSequence(int index)
{
  int i;
  Sequence s;
  int (*comp_ptr)(const void*,const void*) = &InstructionComparator;
  
  for(i=0; i<INSTRUCTIONS_QUANTITY; i++)
  {
    do
    {
      s.ins[i] = rand()%COIL_SADDLES+1;
    }while(IsThereEqualInstruction(&s, i));
    // printf("s.ins[%03d]=%03d,", i, s.ins[i]);
  }
  
  qsort(s.ins, INSTRUCTIONS_QUANTITY, sizeof(int), comp_ptr);
  // printf("\n");
  memcpy(&(sequences[index]), &s, sizeof(Sequence));
}

int IsThereEqualSequences()
{
  int i;
  int thereIs=0;
  
  int (*comp_ptr)(const void*,const void*) = &SequenceComparator;
  qsort(sequences, SEQUENCES_QUANTITY, sizeof(Sequence), comp_ptr);
  
  for(i=0; i<SEQUENCES_QUANTITY-1; i++)
  {
    // if(sequences[i].ins == sequences[i+1].ins)
    if(comp_ptr(sequences[i].ins, sequences[i+1].ins)==0)
    {
      memset(&(sequences[i]), 0x00, sizeof(Sequence));
      memset(&(sequences[i+1]), 0x00, sizeof(Sequence));
      i++;
      thereIs=1;
      printf("EQUAL SEQUENCE!\n");
    }
  }
  return thereIs;
}

void PrintSequence(Sequence *seq)
{
  int i=0;
  for(i=0; i<INSTRUCTIONS_QUANTITY; i++)
    printf("%03d,", seq->ins[i]);
  printf("\n");
}

void PrintSequences()
{
  int i=0;
  for(i=0; i<SEQUENCES_QUANTITY; i++)
    PrintSequence(&(sequences[i]));
}

void ScrambeInstructions()
{
  int i, j, temp, qtty;
  int idx_a, idx_b;
  
  for(i=0; i<SEQUENCES_QUANTITY; i++)
  {
    // qtty = (int)INSTRUCTIONS_QUANTITY/2 + 1;
    qtty = INSTRUCTIONS_QUANTITY;
    
    for(j=0; j<qtty; j++)
    {
      idx_a = rand() % INSTRUCTIONS_QUANTITY;
      idx_b = rand() % INSTRUCTIONS_QUANTITY;
      
      temp = sequences[i].ins[idx_a];
      sequences[i].ins[idx_a] = sequences[i].ins[idx_b];
      sequences[i].ins[idx_b] = temp;
    }
  }
}

int main(int argc, char ** argv)
{
  int i=0;
  memset(sequences, 0x00, SEQUENCES_QUANTITY*sizeof(Sequence));
  
  srand(time(0));
  
  do
  {
    for(i=0; i<SEQUENCES_QUANTITY; i++)
    {
      if(sequences[i].ins[0] == 0)
      {
        GenerateSequence(i);
      }
    }
  }while(IsThereEqualSequences());
  
  // PrintSequences();
  ScrambeInstructions();
  
  // printf("SCRAMBLE\n");
  PrintSequences();
  
  return 0;
}

