# Newton-Image-Interpolation
Using Newton Interpolating Polynomial to resize an Image

# EN_US
Author:
Felipe Pfeifer Rubin
C:128
N: 151050853
email: felipe.rubin@acad.pucrs.br
Class Name: Numerical Methods

Program:
Implemented in C++ language, requires g++ (or clang++) and libpng library to read the image. 
On linux:
sudo apt-get install libpng-dev

On MacOS:
With Homebrew :
brew install libpng

On Windows:
Programs that install tools such as gcc and g++ can also install libpng. 

Compile:

g++ app.cpp -lpng -o app

Execute:

./app <imagem.png> <grau> <porcentagem> 

Example images can be found at TestingImages/

There's also a graphical interface version for Windows, Linux and MacOS but keep in mind that it is in alpha version. As such, some errors might happen.

# PT_BR
Autor:
Felipe Pfeifer Rubin
T:128
Mat: 151050853
email: felipe.rubin@acad.pucrs.br
Cadeira: Métodos Numéricos

Programa:
Implementado em linguagem C++, requer g++ e a biblioteca libpng para leitura da imagem.

No linux:
sudo apt-get install libpng-dev

No MacOS:
Com Homebrew :
brew install libpng

No Windows:
Programas que instalam as ferramentas gcc e g++ possibilitam instalar libpng.


Compilação:

g++ app.cpp -lpng -o app

Execução:

./app <imagem.png> <grau> <porcentagem> 

Imagens exemplo se encontram na pasta TestingImages/

Há também uma versão com interface gráfica para windows, linux e mac. Tenha em mente que está em versão alfa, portanto alguns erros podem aparecer.
