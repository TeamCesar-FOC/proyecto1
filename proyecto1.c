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
// Imprimir restricciones (Sujeto a:)
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

void invermat(int n, float **a, float **ainv, float determ) {

	// Algoritmo para la eliminación simple de Gauss

	int i, j, k;

	float factor,sum;
	//float **L, *D, *X;
	float* D = (float *)malloc(sizeof(float) * n);
	float* X = (float *)malloc(sizeof(float) * n);

    float** L = (float **)malloc(sizeof(float *) * n);
    for(i = 0; i < n; i++) L[i] = (float *)malloc(sizeof(float) * n);

		for (k = 0; k < n - 1; k++) {
			for (i = k+1; i < n;  i++) {
				factor = a[i][k]/a[k][k];
				for (j = k+1; j < n + 1; j++) {
					a[i][j] = a[i][j] - factor * a[k][j];
				}
			}
		}

	// Cálculo del determinante
	determ = 1.;
	for (i = 0; i < n; i++) {
		determ = determ * a[i][i];
	}

	if (determ != 0) {

		// Rutina para determinar las matrices L (inferior) y U (superior) de la
		// descomposición LU
		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				if (i > j) {
					L[i][j] = a[i][j]/a[j][j];
					a[i][j] = 0;
				}
			}
		}

		for (i = 0; i < n; i++) {
			for (j = 0; j < n; j++) {
				L[j][j] = 1;
			}
		}

		// Implementación de la rutina para el cálculo de la inversa
		for (k = 0; k < n; k++) {

			// Esta rutina inicializa los L[i][n] para ser utilizados con la matriz L
			for (i = 0; i < n; i++) {
				if (i == k) L[i][n] = 1;
				else  L[i][n] = 0;
			}

			// Esta función implementa la sustitución hacia adelante con la matriz L y los L[i][n]
			// que produce la rutina anterior
			//float sum;

			D[0] = L[0][n];
				for (i = 1; i < n; i++) {
				sum = 0;
				for (j = 0; j < i; j++) {
					sum = sum + L[i][j]*D[j];
				}
				D[i] = L[i][n] - sum;
			}

			// Esta rutina asigna los D[i] que produce forward para ser utilizados con la matriz U
			for (i = 0; i < n; i++) {
				a[i][n] = D[i];
			}

			// Rutina que aplica la sustitución hacia atras
			X[n-1] = a[n-1][n]/a[n-1][n-1];

			// Determinación de las raíces restantes
			for (i = n - 2; i > -1; i--) {
				sum = 0;
				for (j = i+1; j < n; j++) {
					sum = sum + a[i][j]*X[j];
				}
				X[i] = (a[i][n] - sum)/a[i][i];
			}

			// Esta rutina asigna los X[i] que produce Sustituir como los elementos de la matriz inversa
			for (i = 0; i < n; i++) {
				ainv[i][k] = X[i];
			}
		}   // llave de cierre del for para k
	}   // cierre del if

	free(L);
	free(X);
	free(D);
}//ciere de inverir matriz


void  simplex(float** matrix,int nVar,int nRestrics,float* derRest,float* fObj){
  int i,j;
  float b[nRestrics],A[nRestrics][nVar+nRestrics], C[nVar+nRestrics];//pag5
  float B[nVar][nVar],BI[nVar][nVar];//pag5
  ///////// llenado  b,A,C//////////////
  for (i = 0;i < nRestrics;i++) {
    b[i]=derRest[i];
    C[i]=fObj[i];
    for (j = 0; j < nVar; j++) {
      A[i][j]=matrix[i][j];
    }
    for (j = nVar; j < nVar+nRestrics; j++){
      C[j]=0;
      if(j-nVar==i){
        A[i][j]=1;
      }else{
        A[i][j]=0;
      }
    }
  }
  //---------fin llenado-----//
  ///////////Imprimir/////////
  printf("\nb");
  for (i = 0;i < nRestrics;i++) {
    printf("| %.0f",b[i]);
    printf(" |\n ");
  }

  printf("\nc");
  for (i = 0;i < nVar+nRestrics;i++) {
    printf("| %.0f",C[i]);
  }
  printf(" |\n ");

  printf("\nA");
  for (i = 0;i < nRestrics;i++) {
    printf("|");
    for (j = 0; j < nVar+nRestrics; j++) {
      printf(" %.0f",A[i][j]);
    }
    printf(" |\n ");
  }
  //------Fin Impresion------//
}

void printProblema(float** matrix, int nVar,int nRestrics, char ops[nRestrics], float* derRest, float* fObj, int tipo) {
  int i,j;
  char var = 'x';
  for(i = 0; i < 8 * (nVar + 2); i++) {
    printf("-");
  }
  printf("\n");
  printf("\t");
  for (i = 0; i < nVar; i++) {  // Imprimir variables de referencia
    printf("%c\t", var + i );
    if(var + i == 'z'){
      var = 'p' - i - 1;
    }
  }
  printf("\n");

  printf("%s:\t",tipo==1? "max":"min");  // Imprimir max/min
  for (i = 0; i < nVar; i++) {       // Imprimir coeficientes de la funcion objetivo
    printf("%g\t", fObj[i]);
  }
  printf("\n");

  for (i = 0; i < nRestrics; i++) {
    printf("%s\t", i == 0 ? "sa:" : "");
    for (j = 0; j < nVar; j++) {
      printf("%g\t", matrix[i][j]);   // Coeficientes de restriccion i
    }
    printf(" %c %g\n", ops[i], derRest[i]);   //Operador y lado derecho de restriccion i
  }
  for(i = 0; i < 8 * (nVar + 2); i++) {
    printf("-");
  }
  printf("\n");
}

float** multiplyMatrix(float** matrix1, float** matrix2, int nVar, int nRestrics) {
  float **matrixResult = (float **)malloc(sizeof(float *) * nRestrics);
  for (int i = 0; i < nRestrics; i++) matrixResult[i] = (float *)malloc(sizeof(float) * nVar);
  int max = nVar > nRestrics? nVar : nRestrics;


  for (int i = 0; i < nRestrics; i++) {
		for (int j = 0; j < nRestrics; j++) {
      matrixResult[i][j] = 0; // Inicializar matriz resultado
			for (int k = 0; k < nVar; k++) {
				matrixResult[i][j] += matrix1[i][k] * matrix2[k][j];
			}
		}
	}

  for (int i = 0; i < nRestrics; i++) {
    for (int j = 0; j < nVar; j++) {
      printf("%.2f\t", matrixResult[i][j]);
    }
    printf("\n");
  }
 
  return matrixResult;
}

float** array2matrix(float* array, int len) {
  int i;
  float **matrixResult = (float **)malloc(sizeof(float *) * len);
  for (i = 0; i < len; i++) matrixResult[i] = (float *)malloc(sizeof(float));

  for (i = 0; i < len; i++) {
    matrixResult[i][0] = array[i];
  }

  /*
  for (int i = 0; i < len; i++) {
    printf("%.2f\t", matrixResult[i][0]);
  }
  printf("\n");
  */

  return matrixResult;
}

/////////////////////////////Cuerpo principal///////////////////////////////////
int main() {
  int nVar,nRestrics,i,j;
  char funcObjetivo[MAX_ARRAY];
  int tipo = 0; // Maximizar o minimizar
  float real;
  int fromFile = 0;
  char* varsArray;
  float* fObjetivoCoefsArray;
  char* restOp;
  char rest[MAX_ARRAY]; // Restricciones
  float* restDerArray; // Lado derecho de las restricciones
  // Matriz de coeficientes de las restricciones
  float** restricsMatrix; 
  float** inversa;
  printf("Hola, bienvenido al programa simplex\n");
  printf("1) Ingresar datos por consola.\n");
  printf("2) Cargar datos desde un archivo.\n");
  scanf("%i", &fromFile);

  /* Carga de datos desde archivo:
   * nombre del archivo es: "problema.txt"
   * debe contener:
   * 
   * NumVariables NumRestricciones
   * max/min FuncionObjetivo
   * nFunciones tipoDesigualdad numero
   * 
   * Ej: 
   * 2 2
   * max 2x+3y
   * 5x+6y <= 10
   * 2x+0y <= 5
   * 
   * NOTA: En las restricciones deben ir explicitas los coeficientes de todas las variables y en el mismo orden.
   */
  if (fromFile == 2) {
    char line[MAX_ARRAY];
    FILE *file;
    file = fopen("problema.txt", "r");
    if (file) {
      char ftipo[4];
      fscanf(file, "%i %i\n", &nVar, &nRestrics);
      
      restOp = (char *)malloc(sizeof(char) * nRestrics);  // Operador de la restricción
      restDerArray = (float *)malloc(sizeof(float) * nRestrics);
      restricsMatrix = (float **)malloc(sizeof(float *) * nRestrics);
      for(i = 0; i < nRestrics; i++) restricsMatrix[i] = (float *)malloc(sizeof(float) * nVar);
      inversa = (float **)malloc(sizeof(float *) * nRestrics);
      for(i = 0; i < nRestrics; i++) inversa[i] = (float *)malloc(sizeof(float) * nVar);

      varsArray = getVarsArray(nVar);
      fscanf(file, "%3s %s\n", ftipo, funcObjetivo);
      fObjetivoCoefsArray = getCoefs(funcObjetivo, nVar);
      for (int k = 0; k < nRestrics; k++){
        fscanf(file, "%s %c= %f\n", rest, &restOp[k], &restDerArray[k]);
        restricsMatrix[k] = getCoefs(rest, nVar);
      }
      tipo = ftipo[1] == 'i' ? 2 : 1; // m[i]n 
    }
    fclose(file);
  } else {
    printf("Ingrese cantidad de variables en el problema:\n");
    scanf("%i", &nVar);
    varsArray = getVarsArray(nVar);

    printf("Introduzca funcion objetivo (sin espacios).\n");
    scanf("%s", funcObjetivo);
    fObjetivoCoefsArray = getCoefs(funcObjetivo, nVar);

    TIPO: printf("Seleccione:\n1)Maximizar\n2)Minimizar\n");
    scanf("%i", &tipo);
    if(!(tipo==1 || tipo==2)) goto TIPO;

    printf("Introduzca la cantidad de restricciones del problema:\n");
    scanf("%i", &nRestrics);

    restOp = (char *)malloc(sizeof(char) * nRestrics);  // Operador de la restricción
    restDerArray = (float *)malloc(sizeof(float) * nRestrics);
    restricsMatrix = (float **)malloc(sizeof(float *) * nRestrics);
    for(i = 0; i < nRestrics; i++) restricsMatrix[i] = (float *)malloc(sizeof(float) * nVar);
    inversa = (float **)malloc(sizeof(float *) * nRestrics);
    for(i = 0; i < nRestrics; i++) inversa[i] = (float *)malloc(sizeof(float) * nVar);

    for(i = 0; i < nRestrics; i++){
      if (i == 0) printf("Coloque coeficiente 0 si la variable no aparece. Ej: 2x+0y-5z <= 5\n");
      printf("Introduzca restriccion %i. Utilice ==, >=, <=:\n", i+1); //coma
      scanf("%s %c= %f", rest, &restOp[i], &restDerArray[i]);
      restricsMatrix[i] = getCoefs(rest, nVar);
    }
  }
  // ^ Fin carga de datos ^

  //float **arr = array2matrix(restDerArray, nRestrics);

  float **mult = multiplyMatrix(restricsMatrix, restricsMatrix, nVar, nRestrics);
  // ^ Como esta funcion recibe 2 matrices, en caso de usar un array seria:
  // float **mult = multiplyMatrix(restricsMatrix, array2matrix(restDerArray, nRestrics), nVar, nRestrics);
  


  printProblema(restricsMatrix,nVar,nRestrics,restOp,restDerArray, fObjetivoCoefsArray, tipo);

  simplex(restricsMatrix,nVar,nRestrics,restDerArray, fObjetivoCoefsArray);

  invermat(nVar,restricsMatrix,inversa,real);

  for (i = 0; i < nVar; i++) {
    for (j = 0; j < nVar; j++) {
      printf("%.2f ",inversa[i][j]);
    }
    printf("\n");
  }



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



  // Liberamos memoria dinámica
  for(i = 0; i < nRestrics; i++) free(mult[i]);
  free(mult);
  for(i = 0; i < nRestrics; i++) free(inversa[i]);
  free(inversa);
  for(i = 0; i < nRestrics; i++) free(restricsMatrix[i]);
  free(restricsMatrix);
  free(restDerArray);
  free(restOp);
  free(fObjetivoCoefsArray);
  free(varsArray);

  return 0;
}
