#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_ARRAY 100

// Recibe un arreglo y retorna el subarreglo definido por el intervalo entre begin y end
char* slice(char arr[MAX_ARRAY], int begin, int end) {
    int i, j=0;
  char* arrOut = (char *)malloc(sizeof(char) * MAX_ARRAY); //Array de char de tamaño MAX_ARRAY
  if (end > MAX_ARRAY) {
    printf("Valor end mayor a MAX_ARRAY");
    return arr;
  }
  for (i = begin,j = 0; i<end; i++, j++){
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
    int i;
  char* arrOut = (char *)malloc(sizeof(char) * nVar);
  char var = 'x';
  for (i = 0; i < nVar; i++) {
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
  int i;
  for (i = 0; i < nVar; i++) {// Imprimir variables de referencia
    printf("%c\t", var + i );
    if(var + i == 'z'){
      var = 'p' - i - 1;
    }
  }
  printf("\n");
  for (i = 0; i<nVar; i++) {
    printf("%g\t", coefs[i]);
  }
  printf("\n");
}
// Imprimir restricciones (Subjeto a:)
void printSa(float** matrix, int nVar,int nRestrics, char ops[nRestrics], float* derRest) {
    int i,j;
  char var = 'x';

  for (i = 0; i < nVar; i++) {// Imprimir variables de referencia
    printf("%c\t", var + i );
    if(var + i == 'z'){
      var = 'p' - i - 1;
    }
  }
  printf("\n");
  for(i = 0; i < nRestrics; i++) {
    for(j = 0; j < nVar; j++) {
      printf("%g\t", matrix[i][j]);
    }
    printf(" %c %g\n", ops[i], derRest[i]);
  }
}

void  simplex(float** matrix,int nVar,int nRestrics,float* derRest,float* fObj){
  int i,j;
  float b[nRestrics],a[nRestrics][nVar+nRestrics], c[nVar+nRestrics]; //pag5

  for (i = 0;i < nRestrics;i++) {
    b[i]=derRest[i];
    c[i]=fObj[i];
    for (j = 0; j < nVar; j++) {
      a[i][j]=matrix[i][j];
    }
    for (j = nVar; j < nVar+nRestrics; j++){
      c[j]=0;
      if(j-nVar==i){
        a[i][j]=1;
      }else{
        a[i][j]=0;
      }
    }
  }
  ///////////Imprimir/////////
  printf("\nb");
  for (i = 0;i < nRestrics;i++) {
    printf("| %.0f",b[i]);
    printf(" |\n ");
  }

  printf("\nc");
  for (i = 0;i < nVar+nRestrics;i++) {
    printf("| %.0f",c[i]);
  }
  printf(" |\n ");

  printf("\nA");
  for (i = 0;i < nRestrics;i++) {
    printf("|");
    for (j = 0; j < nVar+nRestrics; j++) {
      printf(" %.0f",a[i][j]);
    }
    printf(" |\n ");
  }
}

void printProblema(float** matrix, int nVar,int nRestrics, char ops[nRestrics], float* derRest, float* fObj, int tipo) {
  int i,j;
  char var = 'x';
  printf("\t");
  for (i = 0; i < nVar; i++) {  // Imprimir variables de referencia
    printf("%c\t", var + i );
    if(var + i == 'z'){
      var = 'p' - i - 1;
    }
  }
  printf("\n");

  printf("%s\t",tipo==1? "max":"min");  // Imprimir max/min
  for (i = 0; i < nVar; i++) {       // Imprimir coeficientes de la funcion objetivo
    printf("%g\t", fObj[i]);
  }
  printf("\n");

  for (i = 0; i < nRestrics; i++) {
    printf("\t");
    for (j = 0; j < nVar; j++) {
      printf("%g\t", matrix[i][j]);   // Coeficientes de restriccion i
    }
    printf(" %c %g\n", ops[i], derRest[i]);   //Operador y lado derecho de restriccion i
  }
}

/////////////////////////////Cuerpo principal///////////////////////////////////
int main() {
  int nVar,i = 0;
  char funcObjetivo[MAX_ARRAY];
  int tipo = 0;
  int nRestrics = 0;

  printf("Hola, bienvenido al programa simplex\n");
  printf("Ingrese cantidad de variables en el problema:\n");
  scanf("%i", &nVar);

  char* varsArray = getVarsArray(nVar);

  printf("Introduzca funcion objetivo (sin espacios).\n");
  scanf("%s", funcObjetivo);

  float* fObjetivoCoefsArray = getCoefs(funcObjetivo, nVar);

  TIPO: printf("Seleccione:\n1)Maximizar\n2)Minimizar\n");
  scanf("%i", &tipo);
  if(!(tipo==1 || tipo==2)) goto TIPO;

  printf("Introduzca la cantidad de restricciones del problema:\n");
  scanf("%i", &nRestrics);

  char rest[MAX_ARRAY]; // Restricciones
  char restOp[nRestrics];  // Operador de la restricción
  float* restDerArray = (float *)malloc(sizeof(float) * nRestrics); // Lado derecho de las restricciones
  // Matriz de coeficientes de las restricciones
  float** restricsMatrix = (float **)malloc(sizeof(float *) * nRestrics);
  for(i = 0; i < nRestrics; i++) restricsMatrix[i] = (float *)malloc(sizeof(float) * nVar);

  for(i = 0; i < nRestrics; i++){
    if (i == 0) printf("Coloque coeficiente 0 si la variable no aparece. Ej: 2x+0y-5z <= 5\n");
    printf("Introduzca restriccion %i. Utilice ==, >=, <=:\n", i+1); //coma
    scanf("%s %c= %f", rest, &restOp[i], &restDerArray[i]);
    restricsMatrix[i] = getCoefs(rest, nVar);
  }
  printf("\n");
  printProblema(restricsMatrix,nVar,nRestrics,restOp,restDerArray, fObjetivoCoefsArray, tipo);

  simplex(restricsMatrix,nVar,nRestrics,restDerArray, fObjetivoCoefsArray);

  /* C -> fObjetivoCoefsArray
   * X -> varsArray
   * A -> restricsMatrix
   * b -> restDerArray
   */

  if (nVar > nRestrics) {
    printf("Este problema no tiene solucion factible. Nro variables mayor al nro restricciones.\n"); //coma
    return 0;
  }

  // Convertimos a forma estandar




  for(i = 0; i < nRestrics; i++) free(restricsMatrix[i]);
  free(restricsMatrix);
  free(restDerArray);
  free(fObjetivoCoefsArray);
  free(varsArray);

  return 0;
}
