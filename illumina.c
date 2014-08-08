/*
GNU GENERAL PUBLIC LICENSE
Author: Pierre Lindenbaum PhD
*/

#include <stdlib.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <ctype.h>
#include <gnumake.h>

#define WHERENL fprintf(stderr,"[%s:%d] ",__FILE__,__LINE__)
#define DEBUG(FormatLiteral, ...)  do { fputs("[DEBUG]",stderr); WHERENL; fprintf (stderr,"" FormatLiteral "\n", ##__VA_ARGS__);} while(0)

/* 
from http://support.illumina.com/help/SequencingAnalysisWorkflow/Content/Vault/Informatics/Sequencing_Analysis/CASAVA/swSEQ_mCA_FASTQFiles.htm

Illumina FASTQ files use the following naming scheme:

<sample name>_<barcode sequence>_L<lane (0-padded to 3 digits)>_R<read number>_<set number (0-padded to 3 digits>.fastq.gz
*/   
enum E_IlluminaComponent
	{
	E_sampleName,
	E_barcodeSequence,
	E_lane,
	E_readNumber,
	E_setNumber
	};

#define NUM_ILLUMINA_COMPONENTS 5

typedef struct illumina_scheme_t
	{
	char* filename;
	char* components[NUM_ILLUMINA_COMPONENTS];
	} IlluminaScheme,*IlluminaSchemePtr ;

static char* gmake_strcpy(const char* src)
	{
	size_t len = strlen(src);
	char* dest = gmk_alloc(len+1);
	memcpy((void*)dest,src,len);
	dest[len]=0;
	return dest;
	}
	
static int is_integer(const char* s)
	{
	char* endptr;
	long v;
	if(s==NULL || s[0]==0) return 0;
	v=strtol(s,&endptr,10);
	return(v>0 && *endptr==0);
	}

static void IlluminaSchemeFree(IlluminaSchemePtr ptr)
	{
	if(ptr==NULL) return;
	gmk_free(ptr->filename);
	free(ptr);
	} 



static IlluminaSchemePtr IlluminaSchemeNew(const char* filename)
	{
	
	IlluminaSchemePtr ptr=NULL;
	size_t i,len = strlen(filename);
	int component_index;
	ptr =(IlluminaSchemePtr)calloc(1,sizeof(IlluminaScheme));
	if(ptr==NULL)
		{
		fprintf(stderr,"out of memory. %s:%d\n",__FILE__,__LINE__);
		exit(EXIT_FAILURE);
		}

	/* create a copy of filename */
	ptr->filename= gmake_strcpy(filename);

	
	/* initialize to '0' */
	for(i=0;i<NUM_ILLUMINA_COMPONENTS;++i)
		{
		ptr->components[i]=&ptr->filename[len];
		}
	
	
	/* sample name can contain a '_', so we need to scan from right to left */
	for(i=len-1,component_index=NUM_ILLUMINA_COMPONENTS-1;
	    (i>0 && component_index>0);
	    i--)
		{
		if(ptr->filename[i]!='_') continue;	
		
		ptr->filename[i]=0;
		ptr->components[component_index]= &(ptr->filename[i+1]);
		//this is last component, remove file extension
		if(component_index+1==NUM_ILLUMINA_COMPONENTS) 
			{
			char* dot=strchr(ptr->components[component_index],'.');
			if(dot==NULL) //not a filename !
				{
				IlluminaSchemeFree(ptr);
				return NULL;
				}
			*dot=0;
			}
		component_index--;
		}
	
	/* setup first remaining component*/
	ptr->components[component_index]= &(ptr->filename[0]);
	/* something wrong happened */
	if(component_index!=0)
		{
		IlluminaSchemeFree(ptr);
		return NULL;
		}
	/* test lane */
	if( ptr->components[E_lane][0]!='L' || !is_integer(&ptr->components[E_lane][1]))
		{
		IlluminaSchemeFree(ptr);
		return NULL;
		}
	/* test readNumber */
	if( !(strcmp(ptr->components[E_readNumber],"R1")==0 || strcmp(ptr->components[E_readNumber],"R2")==0) )
		{
		IlluminaSchemeFree(ptr);
		return NULL;
		}
	/* test set */
	if(!is_integer(ptr->components[E_setNumber]))
		{
		IlluminaSchemeFree(ptr);
		return NULL;
		}
	return ptr;
	}
 

    


/*
 "Every dynamic extension should define the global symbol `plugin_is_GPL_compatible`
 to assert that it has been licensed under a GPL-compatible license"
*/
int plugin_is_GPL_compatible;



static char* illumina_component(const char* filename, int component_index)
     {
     char* ret_value=NULL;
     size_t i,prev;
     char** data=NULL;
     size_t data_size=0UL;
     size_t len=strlen(filename);

     for(i=0,prev=0;i<=len;++i)
     	{
     	if(!(filename[i]==0 || isspace(filename[i]) )) continue;	
	if(prev < i)
		{
		IlluminaSchemePtr scheme;
		char* substr=gmk_alloc((i-prev)+1);
		memcpy((void*)substr,(void*)&filename[prev],(i-prev));
		substr[i-prev]=0;
		scheme= IlluminaSchemeNew(substr);
		if(scheme!=NULL && scheme->components[ component_index][0]!=0) 
			{

			//already found ?
			size_t n=0;
			for(n=0;n< data_size  ;++n)
				{
				if(strcmp(scheme->components[ component_index], data[n])==0)
					{
					//already exists
					break;
					}
				}
			//not found, add it
			if(n==data_size)
				{
				data=(char**)realloc(data,sizeof(char*)*(data_size+1));
				if(data==NULL)
					{
					fprintf(stderr,"out of memory. %s:%d\n",__FILE__,__LINE__);
					exit(EXIT_FAILURE);
					}
				data[data_size] = gmake_strcpy(scheme->components[ component_index]);
				data_size++;
				}
			}
		IlluminaSchemeFree(scheme);
		gmk_free (substr);
		}
	prev=i+1;
	++i;
     	}
     /* nothing to return */
     if(data_size==0) return NULL;	

     /* get length for final string */
     len=0;
     for(i=0;i< data_size;++i) len+=strlen(data[i]);
     /* add lenth for spaces and end of string */
     len+=data_size;
     
     /* build the string */
     ret_value = gmk_alloc(len);
     ret_value[0]=0;
     for(i=0;i< data_size;++i)
     	{
     	if(i>0) strcat( ret_value," ");/* add delimiter */
     	strcat( ret_value,data[i]);
     	}
     
     /*cleanup */
     for(i=0;i< data_size;++i) gmk_free(data[i]);
     free(data);
     
     return ret_value;
     }

#define ILLUMINA_FUNC(FNAME,ITEM)  char* FNAME(const char *nm, unsigned int argc, char **argv)\
     {\
     return (argc!=1?NULL:illumina_component(argv[0],ITEM));\
     } 
    
ILLUMINA_FUNC(illumina_sample,E_sampleName)
ILLUMINA_FUNC(illumina_barcode,E_barcodeSequence)
ILLUMINA_FUNC(illumina_lane,E_lane)
ILLUMINA_FUNC(illumina_read,E_readNumber)
ILLUMINA_FUNC(illumina_set,E_setNumber)

#define ADD_ILLUMINA_FUN(name) gmk_add_function (#name,name, 1, 1, 0);

int illumina_gmk_setup ()
	{
	ADD_ILLUMINA_FUN(illumina_sample);
	ADD_ILLUMINA_FUN(illumina_barcode);
	ADD_ILLUMINA_FUN(illumina_lane);
	ADD_ILLUMINA_FUN(illumina_read);
	ADD_ILLUMINA_FUN(illumina_set);
	return 1;
	}


 
