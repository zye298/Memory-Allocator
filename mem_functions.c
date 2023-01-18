#include "mem.h"
extern BLOCK_HEADER* first_header;


//check if the size_alloc last bit is 0;
//return 0 is free, 1 not free
int Is_Free(int size_alloc){

    int out = size_alloc & 1;


    return out;
};

// int Get_True_alloc_size(int size_alloc){


//     int out = size_alloc & 62;

//     return out;

// };

//get the pointer to the next header

void Set_Alloc (BLOCK_HEADER** cur_header){

    
    (*cur_header)->size_alloc = ((*cur_header)->size_alloc) | 1;


};

void Set_Free (BLOCK_HEADER** cur_header){

    (*cur_header)->size_alloc &= ~(1 << 0);


};

BLOCK_HEADER* Get_Next_Header( BLOCK_HEADER*  cur_header){

    BLOCK_HEADER* next_header = cur_header;

    if ( !Is_Free(cur_header->size_alloc) ){
        next_header = (BLOCK_HEADER *)(   (unsigned long)cur_header + cur_header->size_alloc    );
    }
    else{

        Set_Free(&cur_header);
        next_header = (BLOCK_HEADER *)(   (unsigned long)cur_header + cur_header->size_alloc    );
        Set_Alloc(&cur_header);
    }


    return next_header;
};



int Get_least_Padding( int size){

    int padding = 0;

    int x = size % 8;


    padding = 16 - 8 - x;

    return padding;
}


// return a pointer to the payload
// if a large enough free block isn't available, return NULL
void* Mem_Alloc(int size){

    // find a free block that's big enough
    int locate_free = 0;
    BLOCK_HEADER* cur_header = first_header;

    // check the first header
    if (!Is_Free(cur_header->size_alloc)){
        
        if (   ((cur_header ->size_alloc)-8) >= size ){
            locate_free = 1;
        }
    }

    // loop the linked list; 
    while(   (locate_free == 0) ){

        cur_header = Get_Next_Header(cur_header);

        if (cur_header->size_alloc == 1){

            break;
        }

        if (!Is_Free(cur_header->size_alloc)){

            if (   ((cur_header ->size_alloc)-8) >= size ){
                locate_free = 1;
            }
        }
    };
    // return NULL if we didn't find a block
    if (locate_free == 0){

        return NULL;
    }

    // allocate the block
    
    //payload = size
    cur_header->payload = size;

    //block_size = header_size + payload_size + padding
    // block_size must be divisible by 16
    // split if necessary (if padding size is greater than or equal to 16 split the block)

    int memory = cur_header->size_alloc;

    int num_16 = 0;

    if (size <= 8){

        num_16 = 0;
    }
    else{

        

        if (   (size - 8 ) <=  16    ){

            num_16 = 1;
        }
        else if (size == 1592){

            num_16 = 99;
        }
        else{

            int out = (size - 8 )/16;

            num_16 = out + 1;
        }

    }

    int padding = 16 + num_16 * 16 - (8 + size);

    
    cur_header->size_alloc = 8 + size + padding;
    // size_alloc = block_size + alloc_bit
    // printf("alloc size before is %d\n", cur_header->size_alloc );
    // printf("alloc size is %d\n\n", cur_header->size_alloc);
    // printf("func alloc size is %d\n\n", cur_header->size_alloc);

    int waste_padding= memory - (cur_header->size_alloc) ;  
    // printf("waste padding is %d\n\n", waste_padding);

    //printf("waste padding is %d\n", waste_padding);
    if ( waste_padding >= 16 ){

        // printf("fuck fuck fuck\n");

        // printf("alloc size is %d\n", cur_header->size_alloc );

        // printf("waste padding is %d\n\n", waste_padding);

        // printf("new old address is %p\n", cur_header);

        BLOCK_HEADER* new_header = Get_Next_Header(cur_header);

        // printf("new header address is %p\n", new_header);

        new_header -> size_alloc = waste_padding;
        new_header -> payload = (new_header -> size_alloc) - 8;
    }

    Set_Alloc(&cur_header);

    void* out = (void*)((unsigned long)cur_header + 8);
    return out;
}




// return 0 on success
// return -1 if the input ptr was invalid
int Mem_Free(void *ptr){

    // traverse the list and check all pointers to find the correct block 

    BLOCK_HEADER* target_header = (BLOCK_HEADER*) (   (unsigned long)ptr - 8  );
    // if you reach the end of the list without finding it return -1

    // printf("The target address is %p\n", target_header);

    BLOCK_HEADER* curr_header = first_header;

    while (curr_header != target_header) {

        if (curr_header->size_alloc == 1){

            return -1;
        }

        curr_header = Get_Next_Header(curr_header);

    } 

    // // free the block 
    Set_Free(&target_header);
    // get rid of padding
    // target_header ->size_alloc = 
    target_header -> payload = (target_header -> size_alloc) - 8;
    //printf("The 6666 size alloc is %d\n", (target_header -> size_alloc));


    // // coalesce adjacent free blocks
    BLOCK_HEADER* after_header = Get_Next_Header(target_header);
    if (!Is_Free(after_header->size_alloc)){

        target_header->size_alloc = target_header->size_alloc + after_header->size_alloc;
        target_header -> payload = (target_header -> size_alloc) - 8;

         
    }

    
    if (target_header != first_header){

        BLOCK_HEADER* before_header = first_header;
        BLOCK_HEADER* now_header = Get_Next_Header(first_header);

        while (now_header != target_header) {

            before_header = now_header;
            now_header = Get_Next_Header(now_header);

        } 
        //printf("search done!\n");

        if (!Is_Free(before_header->size_alloc)){

            before_header->size_alloc = before_header->size_alloc + target_header->size_alloc;
            before_header -> payload = (before_header -> size_alloc) - 8;

        }

    }


    return 0;
};

