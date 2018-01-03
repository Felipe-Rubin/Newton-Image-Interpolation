#include <stdio.h>
#include <stdlib.h>
// #include <vector>
#include <iostream>
#include <png.h>
#include <math.h>

// #define DEBUG

using namespace std;

typedef struct{
  unsigned char** data;
  unsigned int width;
  unsigned int height;
  unsigned int bitdepth;
  unsigned int channels;
  unsigned int color_type;
}ImageData;
//


typedef struct{
  unsigned int valid;
  float value;
}FiniteTableValue;

typedef struct{
  FiniteTableValue *column;
  unsigned int length;
}FiniteTableRow;

typedef struct{
  FiniteTableRow *row;
  unsigned int length;
}FiniteTable;

typedef struct{
  FiniteTable *tables;
  unsigned int length;
}FiniteTableList;

//

ImageData original, result;
float mult;
// float **finitetable;

FiniteTableList *ftl;

int quantos;

void printFiniteTablex(/*FiniteTableList ftl*/)
{
  int i,j,k = 0;
  for(i = 0; i < ftl->length; i++){
    printf("FiniteTableList[%d] size: %u\n",i,ftl->tables[i].length);
    for(j = 0; j < ftl->tables[i].length; j++){
      for(k = 0; k < ftl->tables[i].row[j].length; k++){
        printf("[%f,%u], ",ftl->tables[i].row[j].column[k].value,ftl->tables[i].row[j].column[k].valid);
      }
      printf("\n");
    }
  }
}

void printOriginal()
{ 
  for(int i = 0; i < original.height; i++){
      unsigned char *row = original.data[i];
      for(int j = 0; j < original.width; j++){
        if(j+1 < original.width)
          printf("%u, ",row[j]);
        else
          printf("%u",row[j]);
      }
      cout << "\n";
  }
}

void printResult()
{ 
  for(int i = 0; i < result.height; i++){
      unsigned char *row = result.data[i];
      for(int j = 0; j < result.width; j++){
        if(j+1 < result.width)
          printf("%u, ",row[j]);
        else
          printf("%u",row[j]);
      }
      cout << "\n";
  }
}


void deallocate_table_list(FiniteTableList *tablelist)
{
  for(int i = 0; i < tablelist->length; i++){
    FiniteTable table = tablelist->tables[i];
    for(int j = 0; j < table.length; j++){
      FiniteTableRow row = table.row[j];
      free(row.column);

    }
    free(table.row);
  }
  free(tablelist->tables);
  free(tablelist);

}
/*
    ftl.length == original.heigh

    ftl.tables[i].length == original.width
    
    ftl.table[i].row[j].length ==original.width
    
    kyrill's answer to my question:
    (Although a little wrong, helped)

    https://stackoverflow.com/questions/47241718/what-is-the-correct-way-to-allocate-this-nested-structures/47242140#47242140
*/
/* Assuming every table in the list will have num_rows rows and num_columns columns.  */
FiniteTableList * allocate_table_list (int num_rows, int num_columns, int num_tables)
{
  FiniteTableList * res = (FiniteTableList*)malloc (sizeof *res);
  res->tables = (FiniteTable*)malloc (num_tables * sizeof (*res->tables));
  res->length = num_tables;
  for (int t = 0; t < num_tables; t++)
    {
      FiniteTable *table = &res->tables[t];
      table->row = (FiniteTableRow*)malloc (num_rows * sizeof (*table->row));
      table->length = num_rows;
      for (int r = 0; r < num_rows; r++)
        {
          FiniteTableRow *row = &table->row[r];
          // row->column = (FiniteTableValue*)malloc (num_columns * sizeof (*row->column));
          row->column = (FiniteTableValue*)calloc(num_columns,sizeof (*row->column));
          row->length = num_columns;
        }
    }
  return res;
}

void writeResult() {

  FILE *fp = fopen("result.png", "wb");
  if(!fp) abort();

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png){
    png_destroy_read_struct(&png, NULL, NULL);
    fclose(fp);
    // abort();
    return;

  } 

  png_infop info = png_create_info_struct(png);
  if (!info){
    // abort();
    png_destroy_read_struct(&png, &info, NULL);
    fclose(fp);
    return;
  } 

  if (setjmp(png_jmpbuf(png))){
    png_destroy_read_struct(&png, &info, NULL);
    fclose(fp);
    return;
  }

  png_init_io(png, fp);

  // Output is 8bit depth, RGBA format.
  png_set_IHDR(
    png,
    info,
    result.width, result.height,
    8,
    // PNG_COLOR_TYPE_RGBA,
    PNG_COLOR_TYPE_GRAY,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
  );
  png_write_info(png, info);
  png_write_image(png,result.data);
  png_write_end(png, NULL);


  fclose(fp);
  return;
}
void writeOriginal() {
  // int y;

  FILE *fp = fopen("original.png", "wb");
  if(!fp) abort();

  png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
  if (!png){
    png_destroy_read_struct(&png, NULL, NULL);
    fclose(fp);
    // abort();
    return;

  } 

  png_infop info = png_create_info_struct(png);
  if (!info){
    // abort();
    png_destroy_read_struct(&png, &info, NULL);
    fclose(fp);
    return;
  } 

  if (setjmp(png_jmpbuf(png))){
    png_destroy_read_struct(&png, &info, NULL);
    fclose(fp);
    return;
  }

  png_init_io(png, fp);

  // Output is 8bit depth, RGBA format.
  png_set_IHDR(
    png,
    info,
    original.width, original.height,
    8,
    // PNG_COLOR_TYPE_RGBA,
    PNG_COLOR_TYPE_GRAY,
    PNG_INTERLACE_NONE,
    PNG_COMPRESSION_TYPE_DEFAULT,
    PNG_FILTER_TYPE_DEFAULT
  );
  png_write_info(png, info);
  png_write_image(png,original.data);
  png_write_end(png, NULL);

  fclose(fp);
  return;
}

// https://stackoverflow.com/questions/2703255/need-help-using-libpng-to-read-an-image?rq=1
int readImage(string image_name)
{
    png_structp png_ptr;
    png_infop info_ptr;
    FILE *fp;

    if ((fp = fopen(image_name.c_str(), "rb")) == NULL) {
        cout << "Error no file found with that name\n";
        cout << "Maybe you forgot '.png' on image.png ?\n";
        return -1;
    }

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if (png_ptr == NULL) {
        cout << "No idea what happened, bad file buffer probably\n";
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(fp);
        return -1;
    }

    info_ptr = png_create_info_struct(png_ptr);
    if (info_ptr == NULL) {
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        fclose(fp);
        cout << "No idea what happened, bad file chunks probably\n";
        cout <<"Error info_ptr == null\n";
        return -1;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
        fclose(fp);
        cout <<"Error setjmp\n";
        return -1;
    }
    png_init_io(png_ptr, fp);

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_SWAP_ALPHA | PNG_TRANSFORM_EXPAND, NULL);
    ////////////////////////////////////////////////////////////////////
    //IDHR CHUNK
    original.width = png_get_image_width(png_ptr, info_ptr);
    original.height = png_get_image_height(png_ptr, info_ptr);
    original.bitdepth   = png_get_bit_depth(png_ptr, info_ptr);
    original.channels   = png_get_channels(png_ptr, info_ptr);
    original.color_type = png_get_color_type(png_ptr, info_ptr);
    /////////////////////////////////////////////////////////////////////
    if(original.color_type == PNG_COLOR_TYPE_GRAY){
        cout << "It is grayscale\n";
        original.data = (unsigned char**)malloc(sizeof(char**) * original.height);
        png_bytepp aux = png_get_rows(png_ptr,info_ptr);
        for(int i = 0; i < original.height;i++){
          original.data[i] = (unsigned char*)malloc(sizeof(char) * original.width);
          memcpy(original.data[i], aux[i], original.width*sizeof(unsigned char));
        }

        // original.data = png_get_rows(png_ptr, info_ptr);
    }else if(original.color_type == PNG_COLOR_TYPE_RGBA || original.color_type == PNG_COLOR_TYPE_RGB){
      cout << "Converting to grayscale...\n";
      png_bytepp colored_rows = png_get_rows(png_ptr, info_ptr);
      // original.data = (unsigned char**)malloc(sizeof(unsigned char*) * original.height);
      original.data = (unsigned char**)malloc(sizeof(char**) * original.height);
      int pxsize = 3; //RGB R G B
      if(original.color_type == PNG_COLOR_TYPE_RGBA)
        pxsize = 4; //RGBA R G B A
      png_bytep px;
      for(int i = 0; i < original.height; i++){
        // original.data[i] = (unsigned char*)malloc(sizeof(unsigned char*)*original.width);
        original.data[i] = (unsigned char*)malloc(sizeof(char)*original.width);
        unsigned char *row = colored_rows[i];
        unsigned char *originalRow = original.data[i];
        for(int j = 0; j < original.width ;j++){
          px = &(row[j * pxsize]);

          // Generic one... 
          // originalRow[j] = (unsigned int)(0.299 * px[0] +  0.587 * px[1] + 0.114 * px[2]);
          //Use as reference for later
          // The Rehabilitation of Gamma Charles Poynthon
          // Sometimes called Lumma (Luminosity)
          // It gives more weight to the Green, taking into account
          // what color  weights more to the human eye.
          
          // originalRow[j] = (unsigned int)(0.2126 * px[0] +  0.7152 * px[1] + 0.0722 * px[2]);
          // originalRow[j] = (unsigned char)round((float)(0.2126 * px[0] +  0.7152 * px[1] + 0.0722 * px[2]));
          float pxvalue = round((float)(0.2126 * (float)px[0] +  0.7152 * (float)px[1] + 0.0722 * (float)px[2]));
          if(pxvalue >= 255)
            originalRow[j] = 255;
          else if(pxvalue <= 0)
            originalRow[j] = 0;
          else
            originalRow[j] = pxvalue;
        }
        
      }
    }else{
      cout << "I don't know this type of color...\n";
      cout << "I just know GRAYSCALE, RGB, RGBA please try again with one of them.\n";
      return -1;
    }
    // ISSO DAQUI SE EU DESCOMENTAR LIBERA ORIGINAL.DATA...
    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
    fclose(fp);

    return 0;
}
// p(x) = y0 + (x-x0)*(Deltay0/(1!*h^1))+(x-x0)(x-x1)*(Deltay1/(2!*h^2))..



void freeTable(FiniteTable *table)
{
  for(int i = 0; i < table->length; i++){
    FiniteTableRow *rows = table->row;
    for(int j = 0; j < rows->length; j++){
      FiniteTableValue *columns = rows->column;
      free(columns);
    }
    free(rows);
  }
  free(table);
}

void nearest_neighbors(float x,int min,int max,int n,int *ptr){
  int xround = round(x);
  int inicio = 0;
  if((xround - x) > 0){ //Se arredondou para cima
    if(n%2 == 0){//Se for par
      inicio = xround - ((n/2) -1);
    }else{//Impar
      inicio = xround - ((n-1)/2);
    }
  }else{
    if(n%2 == 0){
      inicio = xround - (n/2);
    }else{
      inicio = xround - ((n-1)/2);
    }
  }
  if(inicio < min){ 
    inicio = min;
  }
  // else if(inicio > max){
  else if(inicio+n-1 > max){
   inicio = max - n +1;
 }

  int i = 0;
  // printf("\n");
  // printf("X: %f\n",x);
  // printf("round(x): %d\n",xround);
  // printf("N: %d\n",n);
  // printf("Min: %d\n",min);
  // printf("Max: %d\n",max);
  // printf("inicio: %d\n",inicio);

  // printf("\nX: %f\nround(X): %d\nN: %d\n",x,xround,n);
  for(i = 0; i < n; i++){
    // printf("ptr[%d]: %d\n",i,inicio+i);
    ptr[i] = inicio+i;
  }

}

/*
    Esse initialRow eh o q eu vou usar p/ table->row[initialRow].column[]...
    No caso de de interpolar uma tabela da ftl original, estaria correto usar 
    neighborCols[0] como index, mas depois que ja interpolei cada uma das linhas vizinhas,
    os index de row[.] nao sao por exemplo 4,5,6,7   e sim 0,1,2,3

*/
void interpolate1D(FiniteTable *table, float *x,int *neighborCols,float *value,int initialRow)
{

  // FiniteTableRow prevrow = table.row[j-1];
  // for(int k = 0; k < tablerow.length-j;k++){
  // tablerow.column[k].value = prevrow.column[j+1].value - prevrow.column[j].value;
  // tablerow.column[k].empty = 0;
  // }  
  int i = 0;
  //Primeiro faz os deltas
  float add = 0.0;
  float mult_a = 1.0;
  float mult_b = 1.0;

  // for(i = 0; i < quantos; i++){
  //   table->row[neighborCols[i]]
  // }
  // FiniteTableRow *tablerow = &table->row[neighborCols[0]];
  // add = tablerow->column[0].value;
  
  // for(i = 1; i < quantos; i++){
  //   mult_a *= (*x - (float)neighborCols[i]);
  //   if(mult_a == 0)break;

  //   mult_b *= ((float)i+1); // 1! , 2! , 3! ....

  //   add += (mult_a * tablerow->column[i].value/mult_b);
  // }
  
  // printf("\n");
  
  // add = table->row[0].column[0].value;

  // Esse row[neighborCols[0]] ta dando problema!!!
  // pq o indice de row ta correto quando se trata de uma tabela de ftl,
  // mas Ñ quando ja interpolei as linhas..


  // add = table->row[neighborCols[0]].column[0].value;
  add = table->row[initialRow].column[0].value;


  // printf("BEGI:[add, %f]--[table.length,%d]--[x,%f]--[value,%f]\n",add,table->length,*x,*value);
  // printf("BEGI:[add, %f]--[table.length,%d]--[x,%f]--[value,%f]\n",add,quantos,*x,*value);
  // printf("BEGI:");
  // for(i = 0; i < quantos; i++)printf("[neighborCols[%d],%d]",i,neighborCols[i]);
  // printf("\n");
  // for(i = 1; i < table->length;i++){
  // for(i = 1; i < table->length;i++){
  for(i = 1; i < quantos; i++){
    mult_a = mult_a * (*x - (float)neighborCols[i-1]);
    if(mult_a == 0) break;
    mult_b = mult_b * ((float)i);

    // add += (mult_a * table->row[0].column[i].value/mult_b);

    // add += (mult_a * table->row[neighborCols[0]].column[i].value/mult_b);

    add += (mult_a * table->row[initialRow].column[i].value/mult_b);


    // printf("FORI[i, %d]--[neightborCols[i-1],neightborCols[%d]]--[mult_a,%f]--[mult_b,%f]--[add,%f]\n",i,i-1,mult_a,mult_b,add);
  }
  // printf("STOP:[i, %d]--[neightborCols[i-1],neightborCols[%d]]--[mult_a,%f]--[mult_b,%f]--[add,%f]\n",i,i-1,mult_a,mult_b,add);


  *value = add;
  // printf("Value: %f\n",*value);
  
}

void interpolate2D(unsigned char *coordptr, float *x, float *y, int *neighborRows, int *neighborCols)
{
  // cout << "TUTS TUTS\n";
  // int num_rows, int num_columns, int num_tables
  FiniteTableList *interplist = allocate_table_list(quantos,quantos,1);

  FiniteTable table = interplist->tables[0];

  #ifdef DEBUG
  printf("Table Length Interplist: %u\n",table.length);
  printf("neighborRows{%d,%d,%d,%d}\n",neighborRows[0],neighborRows[1],neighborRows[2],neighborRows[3]);
  printf("neighborCols{%d,%d,%d,%d}\n",neighborCols[0],neighborCols[1],neighborCols[2],neighborCols[3]);
  #endif

  int i,j = 0;
  //gero os Yi's referentes a cada linha
  for(i = 0; i < table.length; i++){
      interpolate1D(&ftl->tables[neighborRows[i]],x,neighborCols,&table.row[i].column[0].value,neighborCols[0]);
      table.row[i].column[0].valid = 1;
      // printf("Value pos-interp: %f\n",table.row[i].column[0].value);
      // table.row[i].length = quantos;
  }
  
  //computando a tabela de colunas interpoladas
  #ifdef DEBUG
    printf("PRE-POINT(%f,%f)\n",*y,*x);
    for(int j = 0; j < table.length; j++){
      FiniteTableRow row = table.row[j];
      for(int k = 0; k < row.length; k++){
        FiniteTableValue tablevalue = row.column[k];
        if(tablevalue.valid == 1)
          printf("[%f,%d]",tablevalue.value,tablevalue.valid);
      }
      printf("\n");
    }
  printf("Table Length: %d\n",table.length);
  #endif

  for(int i = 1; i < table.length; i++){
    for(j = 0; j < table.length-i;j++){
      table.row[j].column[i].value = table.row[j+1].column[i-1].value - table.row[j].column[i-1].value;
      table.row[j].column[i].valid = 1;
    }
  }
  #ifdef DEBUG
    printf("POS-POINT(%f,%f)\n",*y,*x);
    for(int j = 0; j < table.length; j++){
      FiniteTableRow row = table.row[j];
      for(int k = 0; k < row.length; k++){
        FiniteTableValue tablevalue = row.column[k];
        if(tablevalue.valid == 1)
          printf("[%f,%d]",tablevalue.value,tablevalue.valid);
      }
      printf("\n");
    }
  #endif

  float coordptrAux = 0.0;
  #ifdef DEBUG
  printf("neighborRows{%d,%d,%d,%d}\n",neighborRows[0],neighborRows[1],neighborRows[2],neighborRows[3]);
  printf("neighborCols{%d,%d,%d,%d}\n",neighborCols[0],neighborCols[1],neighborCols[2],neighborCols[3]);
  printf("Pre(%f,%f)-->coordptrAux = %f\n",*y,*x,coordptrAux);
  #endif
  interpolate1D(&table,y,neighborRows,&coordptrAux,0);
  #ifdef DEBUG
  printf("Pos(%f,%f)-->coordptrAux = %f\n",*y,*x,coordptrAux);
  #endif
  
  coordptrAux = round(coordptrAux);
  if(coordptrAux > 255)
    *coordptr = (unsigned char)255;
  else if(coordptrAux < 0)
    *coordptr = (unsigned char)0;
  else
    *coordptr = (unsigned char)coordptrAux;



  #ifdef DEBUG
  printf("%d\n",(int)coordptrAux);
  #endif
  
  // deallocate_table_list(interplist);
  deallocate_table_list(interplist);

  
}


void finite_newtonx()
{
  result.height = round((float)original.height  * mult * 0.01);
  result.width = round((float)original.width  * mult * 0.01);

  printf("result height: %u\n",result.height);
  printf("result width: %u\n",result.width);

  
  
  float actualMultHeight = ((float)result.height)/((float)original.height);
  float actualMultWidth = ((float)result.width)/((float)original.width);
  printf("ActualMultHeight %f\n",actualMultHeight);
  printf("ActualMultWidth %f\n",actualMultWidth);

  int i,j = 0;

  // TA CORRETO ISSO? char** ? 
  result.data = (unsigned char**)malloc(sizeof(char**) * result.height);

  int *neighborRows = (int*)malloc(sizeof(int) * quantos);
  int *neighborCols = (int*)malloc(sizeof(int) * quantos);
  //P/ cada linha

  // FiniteTable *table;
  for(i = 0; i < result.height; i++){
    result.data[i] = (unsigned char*)malloc(sizeof(char) * result.width);
    float y = ((((float)i)+1.0)/actualMultHeight)-1.0;
    nearest_neighbors(y,0,original.height-1,quantos,neighborRows);
    for(j = 0; j < result.width;j ++){
      float x = ((((float)j)+1.0)/actualMultWidth)-1.0;

      nearest_neighbors(x,0,original.width-1,quantos,neighborCols);
      //Para cada Linha

      interpolate2D(&(result.data[i][j]),&x,&y,neighborRows,neighborCols);
      
    }
  }

  free(neighborRows);
  free(neighborCols);



}



void finitediffx()
{
  ftl->length = original.height;
  cout << "ftl.length "<<ftl->length <<"\n";
  int i,j = 0;
  FiniteTable *tables = ftl->tables;
  for(i = 0; i < ftl->length; i++){ //Para cada linha da imagem original
    
    tables[i].length = original.width;
    FiniteTableRow *rows = tables[i].row;

    for(j = 0; j < tables[i].length; j++){
      rows[j].length = tables[i].length;
      rows[j].column[0].value = (float)original.data[i][j];
      rows[j].column[0].valid = 1;
    }
    for(int j = 1; j < rows[i].length; j++){
      for(int k = 0; k < rows[j].length-j; k++){
        rows[k].column[j].value = rows[k+1].column[j-1].value - rows[k].column[j-1].value;
        rows[k].column[j].valid = 1;
      }
    }
  }
  #ifdef DEBUG
  for(int i = 0; i < ftl->length; i++){
    FiniteTable table = ftl->tables[i];
    printf("####FiniteTable %d #####\n\n",i);
    for(int j = 0; j < table.length; j++){
      FiniteTableRow row = table.row[j];
      for(int k = 0; k < row.length; k++){

        FiniteTableValue tablevalue = row.column[k];
        if(tablevalue.valid == 1)
          printf("[%f,%d]",tablevalue.value,tablevalue.valid);
          // printf("....\n");
        // else
          // printf("0\n");
      }
      printf("\n");
    }
  }
  #endif
}


int main(int argc, const char **argv)
{
	if(argc < 4){
		cout << "./app <imagem.png> <grau> <percentage>\n";
		return -1;
	}


	string filename = argv[1];
  quantos = atoi(argv[2])+1;
	mult = atof(argv[3]);
  cout << "mult: " << mult <<"\n";
  original = ImageData();
  int i = readImage(filename);
  if(i == -1){
    cout << "Error, please check the information given and try again\n";
    return -1;
  }else{
    cout << "Ok, loaded image information in memory.\n";
    #ifdef DEBUG
    printOriginal();
    #endif
  }

  result = ImageData();


  ftl = allocate_table_list(original.width,original.width,original.height);
  finitediffx();


  #ifdef DEBUG
  printFiniteTablex();
  #endif
  // finite_newton();
  
  finite_newtonx();

  #ifdef DEBUG
  printResult();
  #endif


  writeOriginal();
  writeResult();

  for(int i = 0; i < original.height; i++){
    // for(int j = 0; j < original.width; j++)
    unsigned char* originalRow = original.data[i];
      // free(original.data[i]);
    free(originalRow);
  }
  free(original.data);

  for(int i = 0; i < result.height; i++){
    // for(int j = 0; j < result.width; j++)
    unsigned char* resultRow = result.data[i];
      // free(result.data[i]);
    free(resultRow);
  }
  free(result.data);


  for(int i = 0; i < ftl->length; i++){
    FiniteTable table = ftl->tables[i];
    for(int j = 0; j < table.length; j++){
      FiniteTableRow row = table.row[j];
      free(row.column);
      // for(int k = 0; k < row.length; k++){
      //   FiniteTableValue column = row.column[k];
      //   free(column);
      // }
    }
    free(table.row);
  }
  free(ftl->tables);
  free(ftl);

}




























