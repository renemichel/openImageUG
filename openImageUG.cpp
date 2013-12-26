#include <GL/glut.h>

#include <stdlib.h>
#include<stdio.h>

class Image{
      
      
public:
       Image(char* filename);
       Image(int width, int height, unsigned int color_fill);
       ~Image();
       
       //funciones publicas
       signed int getWidth();
       signed int getHeight();
       unsigned int getPixel(int x, int y);    
       unsigned char getPixelR(int x, int y); 
       unsigned char getPixelG(int x, int y);
       unsigned char getPixelB(int x, int y);
       void save(char* filename);
       void setPixel(unsigned char r, unsigned char g, unsigned char b, int x, int y);
      
private:
        int width;
        int height;
        int depth;
        int file_size;
        FILE* file;
        unsigned int** data;
              
        bool checkBMP();

};


Image::Image(int width, int height, unsigned int color_fill)
{

 int i,j;                 

 this->width = width;
 this->height = height;
 this->depth = 24;
 this->file_size = 0;
 this->file = NULL;
 
 this->data = (unsigned int**)malloc(sizeof(unsigned int*)*this->height);
 
 for(i = 0 ; i < this->height; i++)
       this->data[i] = (unsigned int*)malloc(sizeof(unsigned int)*this->width);
 
 for(i = 0; i < this->width; i++)
 for(j = 0; j < this->height; j++)
       this->data[i][j] = color_fill;
           
}

Image::Image(char* filename)
{
   
 int offset_to_pixels = 0;
 int i = 0;
 int j = 0;
 int padding;  
 int aux;
                   
 //abrimos archivo
 this->file = fopen(filename, "rb");       
 
 if(!this->file)
 {
    printf("File couldnt be read\n");               
    return;
 }
            
 //valores predeterminados
 this->width = 0;
 this->height = 0;
 this->file_size = 0;
 this->depth = 0;
 
 //checamos si es un BMP valido
 if(checkBMP())
    printf("File is a valid BMP Image\n");
 else
 {
        printf("File is not a valid BMP Image\n");
        return;
 }
 
 //leemos bloque de 4 bytes que contiene el tamaño del archivo
 fread(&this->file_size, 4, 1, this->file);
 printf("File size: %d bytes\n", this->file_size);  
 
 //movemos el cursos hasta el byte 10 para poder leer la propiedad q indica el offset hasta donde se encuentran los pixeles
 //esto debido a que el header del archivo contiene demasiados datos que vamos a ignorar, lo q nos importa
 //es la informacion de pixeles
 fseek(this->file, 10, SEEK_SET);
 fread(&offset_to_pixels, 4, 1, this->file);
 printf("Offset to Pixels: %d bytes\n", offset_to_pixels);  
 
 //movemos el cursor 4 bytes (despreciamos la informacion en los siguietnes 4 bytes)
 fseek(this->file, 4, SEEK_CUR);
 
 //leemos el width
 fread(&this->width, 4, 1, this->file);
 printf("Width: %d\n", this->width);  
 
 
  //leemos el heigth
 fread(&this->height, 4, 1, this->file);
 printf("Height: %d\n", this->height);  
 
 //movemos el cursor 2 bytes (despreciamos la informacion en los siguietnes 2 bytes)
 fseek(this->file, 2, SEEK_CUR);
 
 //leeemos el depth
 fread(&this->depth, 2, 1, this->file);
 printf("Depth: %d bits\n", this->depth);
 
 
 //Con los datos obtenidos podemos asignar memoria para la informacion de los pixeles
 this->data = (unsigned int**)malloc(sizeof(unsigned int)*this->height);
 for(i = 0; i < this->height; i++) 
    this->data[i] = (unsigned int*)malloc(sizeof(unsigned int)*this->width);
    

//Movemos el cursor al inicio de los datos de los pixeles
fseek(this->file, offset_to_pixels, SEEK_SET);
 
 
 //calculamos el padding
  if( (this->width*3) % 4 != 0)
  padding = 4-((this->width*3) % 4);
  else
  padding = 0;
  printf("Padding: %d bytes\n",padding);
  
  
 //Leemos los datos, (Segun el formato los pixeles estan guardados empezando con la ultima fila y la primer columna)
 for(i = height - 1; i >= 0; i--)
 {
 for(j = 0; j < width; j++)
 {      
        fread(&this->data[i][j], 3, 1, this->file);
 }
 //leemos el padding que descartamos
 
  fread(&aux, padding, 1, this->file);
 }
       
 //for(i = 0; i < height; i++)
// for(j = 0; j < width; j++)
// {
//        printf("Pixel[%d][%d] = %06X\n", i,j,this->data[i][j]);
//        r = (this->data[i][j]& 0xFF0000)>>16;
//         g = (this->data[i][j]& 0x00FF00)>>8;
//         b = (this->data[i][j]& 0x0000FF);
//         
//         printf("%d %d %d\n",r,g,b);
//         
//       //  getchar();
//}

   fclose(this->file);
}

Image::~Image()
{
  //fclose(this->file);
}

void Image::save(char* filename)
{
     char header[2] = {'B', 'M'};
     int padding = 0;
     int filesize = 0;
     int aux = 0;
     int offset = 54;
     int headersize = 40;
     int colorplanes = 1;
     int compression = 0;
     int i,j;
     
      if( (this->width*3) % 4 != 0)
      padding = 4-((this->width*3) % 4);
      else
      padding = 0;
     
     filesize = 54 + ((this->width*3)+padding)*this->height;
     
     
      this->file = fopen(filename,"wb");
      
      //escribimos BM
      fwrite(header, 2,1,this->file);
      
      //escribimos el tamaño del archivo
      fwrite(&filesize, 4, 1, this->file);
      
      //escribijmos 4 bytes reservados
      fwrite(&aux, 4, 1, this->file);
      
      //escribimos el offset
      fwrite(&offset, 4, 1, this->file);
      
      //escribimos el headersize
      fwrite(&headersize, 4, 1, this->file);
      
      //escribimos el width
      fwrite(&this->width, 4, 1, this->file);
      
      //escribimos el height
      fwrite(&this->height, 4, 1, this->file);
      
      //escribimos los color planes
      fwrite(&colorplanes, 2, 1, this->file);
      
      //escribimos el depth
      fwrite(&this->depth, 2, 1, this->file);
      
      //escribnimos la copmresion
      fwrite(&compression, 4, 1, this->file);
      
      //escribimos los valores faltantes q ignoramos
      fwrite(&aux, 4, 1, this->file);
      fwrite(&aux, 4, 1, this->file);
      fwrite(&aux, 4, 1, this->file);
      fwrite(&aux, 4, 1, this->file);
      fwrite(&aux, 4, 1, this->file);
      
       //escribimos los datos
     for(i = height - 1; i >= 0; i--)
     {
     for(j = 0; j < width; j++)
     {      
            fwrite(&this->data[i][j], 3, 1, this->file);
     }
     //leemos el padding que descartamos
     
      fwrite(&aux, padding, 1, this->file);
     }
      
      
      
      fclose(this->file);
}

void Image::setPixel(unsigned char r, unsigned char g, unsigned char b, int x, int y)
{
     unsigned int color = (int)(r << 16) + (int)(g << 8) + (int)b;
     this->data[x][y] = color;
}

unsigned char Image::getPixelR(int x, int y)
{
  return (this->data[x][y] & 0xFF0000)>>16;        
}


unsigned char Image::getPixelG(int x, int y)
{
  return (this->data[x][y] & 0x00FF00)>>8;        
}

unsigned char Image::getPixelB(int x, int y)
{
  return (this->data[x][y] & 0x0000FF);        
}

int Image::getWidth()
{
 return this->width;         
}

int Image::getHeight()
{
 return this->height;         
}

bool Image::checkBMP()
{
     char header[3]; //Leera el identificador BM , el tercer caracter es para el caracter nulo

     printf("Checking BMP header... :");
     
     //Leemos los primeros 2 bytes de la imagen
     fread(header, 2, 1, this->file);
     
     //El tercer caracter a nulo
     header[2] = '\0';
     printf("%s\n",header);
     
     if(header[0] != 'B' || header[1] != 'M')
        return false; 
     else
        return true; 
}

unsigned int Image::getPixel(int x, int y)
{
 return this->data[x][y];         
}

Image img = NULL;

static void display(void)
{
    int i = 0;
    int j = 0;    
    
    unsigned char r = 0;
    unsigned char g = 0;
    unsigned char b = 0;

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glBegin(GL_POINTS);

    for(i = 0; i < img.getHeight(); i++)
    for(j = 0; j < img.getWidth(); j++)
    {
         r = (img.getPixel(i,j) & 0xFF0000)>>16;
         g = (img.getPixel(i,j) & 0x00FF00)>>8;
         b = (img.getPixel(i,j) & 0x0000FF);
         
         //printf("%d %d %d\n",r,g,b);
         //getchar();

         glColor3ub(r,g,b);     
         glVertex2f(j,i);
    }

      
    glEnd();
    glFlush();

}


void
reshape(int w, int h)
{
  /* Cambiamos el sistema de coordenadas cartesianas 3D al
  sistema de coordenadas 2D de las ventanas donde el pixel 0,0
  se encuentra ubicado en la esquina superior izquierda.
  Y donde la coordenada y va incrementando hacia abajo
   */

  glViewport(0, 0, w, h);       
  glMatrixMode(GL_PROJECTION);  
  glLoadIdentity();             
  glOrtho(0, w, 0, h, -1, 1);   
  glScalef(1, -1, 1);           
  glTranslatef(0, -h, 0);       
}


/* Program entry point */

int main(int argc, char *argv[])
{
    int i,j;

    //Abrimos la imagen
    img = Image("test5.bmp");  
   
   //Para comprobar la escritura de la imagen, modificamos la imagen recien abierta
   // y le sacamos el negativo a una seccion
   // esto se logra restando 255 a los 3 canales y obteniendo su absoluto
    for(i = 0; i < img.getHeight()/2; i++)
    for(j = 0; j < img.getWidth()/2; j++)
    img.setPixel(abs(img.getPixelR(i,j)-255), abs(img.getPixelG(i,j)-255), abs(img.getPixelB(i,j)-255), i,j);   
   
   //guardamos la nueva imagen con un nombre distingo
    img.save("new.bmp");  
    glutInit(&argc, argv);
    glutInitWindowSize(img.getWidth(),img.getHeight());
    glutInitWindowPosition(10,10);
    

    glutCreateWindow("Proyecto Final - Sofia Flores Hernandez");

   
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
   

    glClearColor(1,1,1,1);

    glutMainLoop();

    return EXIT_SUCCESS;
}

