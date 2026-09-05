#include<stdio.h>
#include<conio.h>

void display(int element,int no);
int add(int no);
int search(int element,int no);
int menu();
int arr[100],i;

void display(int element,int no)
{
	for(i=0;i<no;i++)
	{
		if(arr[i]==element)
		{
			printf("-->\t\a%d\n",arr[i]);
		}
		else
		{
			printf("   \t%d\n",arr[i]);
		}
	}
}

int add(int no)
{
	for(i=0;i<no;++i)
	{
		//clrscr();
		printf("%d>\t\a",i+1);
		scanf("%d",&arr[i]);
	}
	return 0;
}

int search(int element,int no)
{
	for(i=0;i<no;i++)
	{
		if(arr[i]==element)
		{
			printf("The element %d found at location %d\n\n",element,i+1);
		}
		else
		{
		//	printf("No query found");
		}
	}
}

void write2file(int no,int element)
{
	FILE *fp;
	fp=fopen("debagnik.txt","w");
	for(i=0;i<no;i++)
	{
		if(arr[i]==element)
		{
			fprintf(fp,"-->\t\a%d\n",arr[i]);
		}
		else
		{
			fprintf(fp,"   \t%d\n",arr[i]);
		}
	}
	fclose(fp);
}

int menu()
{
	int opt;
	printf("Press 1 to enter new set of data\nPress 2 search an element\nPress 3 to point the element on the map\nPress 4 to save the search and export\nPress 5 to exit\nPrompt:\a\t");
	scanf("%d",&opt);
	printf("\n\n");
	return opt;
}

void main()
{
	int prompt=0,element,no;
	do
	{
		prompt=menu();
		switch(prompt)
		{
			case 1:
				printf("Enter the number of data:\t");
				scanf("%d",&no);
				add(no);
				break;
			case 2:
				printf("Enter the number to search:\a\t");
				scanf("%d",&element);
				search(element,no);
				break;
			case 3:
				display(element,no);
				menu();
				break;
			case 4:
				write2file(no,element);
				printf("file saved sucessfully to avoid overwritng data please rename the file");
				break;
			case 5:
				break;
			default :
				printf("No query found enter again");
				break;
		}
	}while(prompt!=5);
	getch();
}
