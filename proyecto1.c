#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_ARRAY 100

// Recibe un arreglo y retorna el subarreglo definido por el intervalo entre begin y end
char* slice(char arr[MAX_ARRAY], int begin, int end) {
  char* arrOut = (char *)malloc(sizeof(char) * MAX_ARRAY); //Array de char de tamaño MAX_ARRAY
  if (end > MAX_ARRAY) {
    printf("Valor end mayor a MAX_ARRAY");
    return arr;
  }
  for (int i = begin, j = 0; i<end; i++, j++){
    arrOut[j]=arr[i];
  }
  return arrOut;
}

// Recibe una funcion y retorna un arreglo con los coeficientes
float* getCoefs(char arr[MAX_ARRAY], int nVar){
  float* coefs = (float *)malloc(sizeof(float) * nVar); //Array de int de tamaño nVar
  int short is1stDigit = 1;
  int nVarCompleted = 0;
  int i = 0;
  int posIni = -1;
  int posFin = -1;
  char oper = 'n';

  while (arr[i] != '\0') {
    if (is1stDigit && isdigit(arr[i])) {
      if (nVarCompleted == 0) {
        is1stDigit = 0;
        posIni = i;
        if (oper == 'n') {
          oper = '+';
        }
      } else if (oper != 'n') {
        is1stDigit = 0;
        posIni = i;
      }
    } else if (oper != 'n' && is1stDigit && isdigit(arr[i])) {
      is1stDigit = 0;
      posIni = i;
    } else if (!is1stDigit && !isdigit(arr[i])) {
      posFin = i;
    } else if (arr[i] == '+' || arr[i] == '-') {
      oper = arr[i];
    } else if (oper == 'n' && !isdigit(arr[i])) {
      coefs[nVarCompleted] = 1;
      nVarCompleted++;
    } else if (!isdigit(arr[i])) {
      coefs[nVarCompleted] = oper == '-' ? -1 : 1; 
      nVarCompleted++;
      oper = 'n';
    }
    // Si ya tiene la posicion final e inicial de un coeficiente y su signo, lo extrae
    if (posIni >= 0 && posFin >= 0 && oper != 'n') {
      char* aux = slice(arr, posIni, posFin);
      int j = 0;
      int coef = 0;
      while (aux[j] != '\0') {
        coef = coef * 10;
        coef = coef + (int)aux[j] - '0';
        j++;
      }
      coef = oper == '+'? coef : coef * -1;
      coefs[nVarCompleted] = coef;
      posIni = -1;
      posFin = -1;
      oper = 'n';
      is1stDigit = 1;
      nVarCompleted++;
      free(aux);
    }

    if(nVarCompleted == nVar){
      return coefs;
    }
    i++;
  }

}

// Recibe el numero de variables y retorna un arreglo con el nombre de cada variable que el programa usará
char* getVarsArray (int nVar) {
  char* arrOut = (char *)malloc(sizeof(char) * nVar);
  char var = 'x';
  for (int i = 0; i < nVar; i++) {
    arrOut[i] = var + i;
    if(var + i == 'z'){
      var = 'p' - i - 1;
    }
  }
  return arrOut;
}

// Recibe un arreglo de coeficientes y los imprime con su respectivo nombre de variable
void printCoefs(float* coefs, int nVar) {
  char var = 'x';
  for (int i = 0; i < nVar; i++) {// Imprimir variables de referencia
    printf("%c\t", var + i );
    if(var + i == 'z'){
      var = 'p' - i - 1;
    }
  }
  printf("\n");
  for (int i = 0; i<nVar; i++) {
    printf("%g\t", coefs[i]);
  } 
  printf("\n");
}
// Imprimir restricciones (Subjeto a:)
void printSa(float** matrix, int nVar,int nRestrics, char ops[nRestrics], float* derRest) {
  char var = 'x';
  for (int i = 0; i < nVar; i++) {// Imprimir variables de referencia
    printf("%c\t", var + i );
    if(var + i == 'z'){
      var = 'p' - i - 1;
    }
  }
  printf("\n");
  for(int i = 0; i < nRestrics; i++) {
    for(int j = 0; j < nVar; j++) {
      printf("%g\t", matrix[i][j]);
    }
    printf(" %c %g\n", ops[i], derRest[i]);
  }
}

void printProblema(float** matrix, int nVar,int nRestrics, char ops[nRestrics], float* derRest, float* fObj, int tipo) {
  char var = 'x';
  printf("\t");
  for (int i = 0; i < nVar; i++) {  // Imprimir variables de referencia
    printf("%c\t", var + i );
    if(var + i == 'z'){
      var = 'p' - i - 1;
    }
  }
  printf("\n");

  printf("%s\t",tipo==1? "max":"min");  // Imprimir max/min
  for (int i = 0; i < nVar; i++) {       // Imprimir coeficientes de la funcion objetivo
    printf("%g\t", fObj[i]);
  }
  printf("\n");

  for (int i = 0; i < nRestrics; i++) {
    printf("\t");
    for (int j = 0; j < nVar; j++) {
      printf("%g\t", matrix[i][j]);   // Coeficientes de restriccion i
    }
    printf(" %c %g\n", ops[i], derRest[i]);   //Operador y lado derecho de restriccion i
  }
}


int main() {
  int nVar = 0;
  char funcObjetivo[MAX_ARRAY];
  int tipo = 0;
  int nRestrics = 0;

  printf("Hola.\n");
  printf("¿Cantidad de variables en el problema?\n");
  scanf("%i", &nVar);

  char* varsArray = getVarsArray(nVar);

  printf("Introduzca función objetivo (sin espacios).\n");
  scanf("%s", funcObjetivo);

  float* fObjetivoCoefsArray = getCoefs(funcObjetivo, nVar);

  TIPO: printf("Seleccione:\n1)Maximizar\n2)Minimizar\n");
  scanf("%i", &tipo);
  if(!(tipo==1 || tipo==2)) goto TIPO;

  printf("Introduzca la cantidad de restricciones del problema.\n");
  scanf("%i", &nRestrics);

  char rest[MAX_ARRAY]; // Restricciones
  char restOp[nRestrics];  // Operador de la restricción
  float* restDerArray = (float *)malloc(sizeof(float) * nRestrics); // Lado derecho de las restricciones
  // Matriz de coeficientes de las restricciones
  float** restricsMatrix = (float **)malloc(sizeof(float *) * nRestrics); 
  for(int i = 0; i < nRestrics; i++) restricsMatrix[i] = (float *)malloc(sizeof(float) * nVar);
  
  for(int i = 0; i < nRestrics; i++){
    printf("Introduzca restricción %i. Utilice ==, >=, <=\n", i+1);
     if (i == 1) printf("Coloque coeficiente 0 si la variable no aparece. Ej: 2x+0y-5z <= 5\n");
    scanf("%s %c= %f", rest, &restOp[i], &restDerArray[i]);
    restricsMatrix[i] = getCoefs(rest, nVar);
  }
  printf("\n");
  printProblema(restricsMatrix,nVar,nRestrics,restOp,restDerArray, fObjetivoCoefsArray, tipo);

  /* C -> fObjetivoCoefsArray 
   * X -> varsArray
   * A -> restricsMatrix
   * b -> restDerArray
   */

  if (nVar > nRestrics) {
    printf("Este problema no tiene solución factible. Nro variables mayor al nro restricciones.\n");
    return 0;
  }

  // Convertimos a forma estandar
  



  for(int i = 0; i < nRestrics; i++) free(restricsMatrix[i]);
  free(restricsMatrix);
  free(restDerArray);
  free(fObjetivoCoefsArray);
  free(varsArray);

  return 0;
}