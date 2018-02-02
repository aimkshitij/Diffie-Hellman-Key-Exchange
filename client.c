#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>

int  prime = 0;
int  primitiveRoot = 0;

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




bool millerTest(long long int d,long long int n){
  srand(time(NULL));
  long long int a = 2 + rand() % (n - 4);
  long long int x = power(a,d,n);
  if (x == 1|| x == n-1){
    return true;
  }
  while(d != n-1){
		x = (x*x)%n;
		d *= 2;
		if (x == 1){
      return false;
    }
		if (x == n-1){
      return true;
    }
	}
	return false;
}











// check wehter number is prime or not with miller rabin test
bool isPrime(long long int n){
   if (n <= 1 || n == 4){
     return false;
   }
   if (n <= 3){
     return true;
   }

   // Find r(remainder) such that n = 2^d * r + 1 for some remainder >= 1
   long long int d = n - 1;
   while (d % 2 == 0){
     d = d/2;
   }
    // Perform MillerTest millerIteration times
   int iter = 10;
    while(iter--){
      if (millerTest(d,n) == false){
        return false;
      }
    }
    return true;
}

int generateRandomPrime(){
  srand(time(NULL));
  while(1){
		int checkPrime = rand() % 10000;
    if(isPrime(checkPrime)){
      return checkPrime;
    }
  }
}



int findPrimitiveRoot(){
	int arr[100000];
	int phi = prime -1;
	int n = phi;
	int j=0;
	for(int i=2;i*i<=n;++i){
		if(n%i==0){
			arr[j] = i;
			j++;
			while(n%i==0){
				n=n/i;
			}
		}
	}
	if(n>1){
		arr[j]=n;
	}

	for (int res=2; res<=prime; ++res) {
        bool ok = true;
        for (int  i=0; i<j && ok; ++i)
            ok &= power (res, phi / arr[i], prime) != 1;
        if (ok)  return res;
    }
	// for(int i=2;i<=prime;i++){
	// 	bool ok = true;
	// 	for(int k=0;k<j;&&ok;k++){
	// 		ok &=power(i,phi/arr[i],prime)!=1;
	// 	}
	// 	if(ok) return i;
	// }
	return -1;
}





int main(int argc , char *argv[]){
	if (argc<3){
		printf("Enter Valid Number of Argumnets (CompliedFile , Server IP, InputFile)\n");
		exit(0);
	}
	char *pText = NULL;
	size_t  size = 0;

	FILE *plaintext;
	if ((plaintext = fopen(argv[2], "r")) == NULL) {
		perror("Error ");
		exit(-1);
	}

	fseek(plaintext,0,SEEK_END);
	size =ftell(plaintext);
	rewind(plaintext);

	pText = (char*)malloc((size+1)*sizeof(*pText));
	fread(pText,size,1,plaintext);
	pText[size-1] ='\0';

	char cipherText[size]="";
	cipherText[size] = '\0';
	int  index=0;

	printf("----Alice---- \n");
  prime = generateRandomPrime();
  // printf("Prime Number is %d \n",prime);
	primitiveRoot = findPrimitiveRoot();
	// printf("Primitive Root is %d \n",primitiveRoot);
	int  Xa = rand()%(prime-1) + 2;
	// printf("Client Private Key %d \n",Xa);
	int  Ya = power(primitiveRoot,Xa,prime);
	printf("Prime is %d\n",prime);
	printf("Primitive Root is %d\n",primitiveRoot);
	// printf("Client Public Key %d \n",Ya);
	printf("Alice's Public Keys is %d \n",Ya);
	int  sockfd;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("Error ");
		exit(-1);
	}
	struct sockaddr_in serv_addr;
	memset(&serv_addr, 0, sizeof(serv_addr));
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
	serv_addr.sin_port = htons(10000);



	if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
		perror("Error ");
		exit(-1);
	}

	char buffer[1024];
	memset(buffer,0,sizeof(buffer));
	sprintf(buffer,"%d,%d,%d",Ya,prime,primitiveRoot);
	send(sockfd, buffer, sizeof(buffer), 0);


	memset(buffer,0,sizeof(buffer));
	recv(sockfd,buffer,1024,0);
	int  Yb = atoi(buffer);
	printf("BoB's Public Key is %d \n",Yb);

	int  secretKey = power(Yb,Xa,prime);
	printf("Secret Key is %d \n",secretKey);

	// send(sockfd,&Ya, sizeof(int),0);
	// int publicKey = htonl(Ya);
	// printf("Sent Received %d ",Ya);
	// char *message= (char*)&publicKey;
	// int val = atoi(message);
	// printf("Sent Received %d ",val);
	// printf("Message is %s",message);
	// // send(sockfd,message,n,0);
	// // rc = write(fd, data, left);
	// write(sockfd, message, sizeof(message));

	char encodingTable[] = {' ','A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',
													',','.','?','0','1','2','3','4','5','6','7','8','9',
													'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z','!'};


	memset(buffer,0,sizeof(buffer));

	printf("Actual Plaintext is:- %s\n", pText);

	for(int i=0;i<strlen(pText);i++){
		if(pText[i]>='a' && pText[i]<='z'){
			index = (pText[i]-'a'+ 40 + secretKey)%67;
			cipherText[i]=encodingTable[index];
		}
		else if(pText[i]>='A' && pText[i]<='Z'){
			index = (pText[i]-'A'+ 1 + secretKey)%67;
			cipherText[i]=encodingTable[index];
		}
		else if(pText[i]>='0' && pText[i]<='9'){
			index = (pText[i]-'0'+ 30 + secretKey)%67;
			cipherText[i]=encodingTable[index];
		}
		else if(pText[i]=='!'){
			index = (66 + secretKey)%67;
			cipherText[i]=encodingTable[index];
		}
		else if(pText[i]==' '){
			index = (secretKey)%67;
			cipherText[i]=encodingTable[index];
		}
		else if(pText[i]==','){
			index = (27+secretKey)%67;
			cipherText[i]=encodingTable[index];
		}
		else if(pText[i]=='.'){
			index = (28+secretKey)%67;
			cipherText[i]=encodingTable[index];
		}
		else if(pText[i]=='?'){
			index = (29+secretKey)%67;
			cipherText[i]=encodingTable[index];
		}
		else{
			cipherText[i]=pText[i];
		}
	}

	printf("Sent cipherText to Bob:- %s\n",cipherText);
	send(sockfd,cipherText,size,0);
	fclose(plaintext);
	close(sockfd);
  return 0;
}
