#include <time.h>
#include <limits.h>
#include <arpa/inet.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>


using namespace std;

#define MAXCLIENT 10

// ALICE                                                 BoB
// 3^15 mod 17 = 6                                   3^13 mod 17 = 12
// 12                                                 6
// 12^15 mod 17                                      6^13 mod 17




long long int power(long long int a,long long int d, long long int n){
	long long int p = 1;
	a = a % n;
	while(d>0){
		if(d & 1){
      p = (p*a) % n;
    }
		d = d/2;
		a = (a*a)%n;
	}
	return p;
}


int main(int argc , char *argv[]){
	if(argc < 2){
		printf("Enter Valid Number of Argumnets (CompliedFile , Server IP)\n");
		exit(0);
	}
  printf("----Bob----\n");
  int serverSock,ClienSock;
  if((serverSock = socket(AF_INET, SOCK_STREAM, 0))== -1){
		perror("Server socket: ");
		exit(-1);
	}

  struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(10000);

  if (bind(serverSock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("BIND ");
		exit(-1);
	}
  if(listen(serverSock, MAXCLIENT) == -1){
    perror("listen ");
    exit(-1);
  }

  if((ClienSock = accept(serverSock, NULL, NULL)) == -1){
    perror("accept ");
    exit(-1);
  }


  char buffer[1024];
  char cipherText[1024];
  memset(buffer,0,sizeof(buffer));

  recv(ClienSock,buffer,1024,0);
	int Ya;
	int prime ;
	int primitiveRoot;
	sscanf(buffer,"%d,%d,%d",&Ya,&prime,&primitiveRoot);

  printf("Prime is %d\n",prime);
  printf("Primitive Root is %d\n",primitiveRoot);


  long long int Xb = rand()%(prime-1) + 2;
  int Yb = power(primitiveRoot,Xb,prime);

  printf("Bob's Public Key is %d\n",Yb);
  printf("Alice's Public Key is %d \n",Ya);


  memset(buffer,0,sizeof(buffer));
  sprintf(buffer,"%d",Yb);
  send(ClienSock, buffer, sizeof(buffer), 0);

  int secretKey = power(Ya,Xb,prime);
	printf("Secret Key is %d\n",secretKey);

  memset(buffer,0,sizeof(buffer));

  int len = recv(ClienSock,cipherText,1024,0);
  printf("Recieved cipherText from Alice:- %s\n",cipherText);
  cipherText[len-1] = '\0';


  char decodingTable[] = {' ','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
													',','.','?','0','1','2','3','4','5','6','7','8','9',
													'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','!'};


  char plainText[strlen(cipherText)]="";

  // printf("%d %d\n",len,(strlen(cipherText)));

  int index =0;
  for(int i=0;i<strlen(cipherText);i++){
    if(cipherText[i]>='a' && cipherText[i]<='z'){
			index = (cipherText[i]-'a'+ 40 - secretKey)%67;
      if(index<0)
        index = index + 67;

			plainText[i]=decodingTable[index];
		}
		else if(cipherText[i]>='A' && cipherText[i]<='Z'){
			index = (cipherText[i]-'A'+ 1 - secretKey)%67;
      if(index<0)
        index = index + 67;
			plainText[i]=decodingTable[index];
		}
		else if(cipherText[i]>='0' && cipherText[i]<='9'){
			index = (cipherText[i]-'0'+ 30 - secretKey)%67;
      if(index<0)
        index = index + 67;
			plainText[i]=decodingTable[index];
		}
		else if(cipherText[i]=='!'){
			index = (66 - secretKey)%67;
      if(index<0)
        index = index + 67;
			plainText[i]=decodingTable[index];
		}
		else if(cipherText[i]==' '){
			index = (0-secretKey)%67;
      if(index<0)
        index = index + 67;
			plainText[i]=decodingTable[index];
		}
		else if(cipherText[i]==','){
			index = (27-secretKey)%67;
      if(index<0)
        index = index + 67;
			plainText[i]=decodingTable[index];
		}
		else if(cipherText[i]=='.'){
			index = (28-secretKey)%67;
      if(index<0)
        index = index + 67;
			plainText[i]=decodingTable[index];
		}
		else if(cipherText[i]=='?'){
			index = (29-secretKey)%67;
      if(index<0)
        index = index + 67;
			plainText[i]=decodingTable[index];
		}
		else{
			plainText[i]=cipherText[i];
		}
  }

  plainText[len-1]='\0';
  printf("Actual PlainText from Alice:- %s",plainText);
  printf("\n");
  FILE *plainT;
  plainT = fopen("output.txt", "ab+");
  if(plainT){ //if file does not exist, create it
    plainT=fopen("output.txt", "wb");
  }
  fprintf(plainT, "%s",plainText);

  fclose(plainT);
	printf("\nOutput File Generated\n");
  close(ClienSock);
  close(serverSock);

}
