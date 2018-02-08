#include <moca.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <mocaerr.h>
#include <mocagendef.h>
#include <mislib.h>

#include <rfiderr.h>
#include <rfidgendef.h>
#include <rfiderr.h>
#include <taglib.h>

#define DEFAULT_KEEP_COUNT 3
#define DEFAULT_KEEP_TIME  3

typedef struct fifoList
{
    struct fifoList *next;

    tagInfo *tagList;

    time_t timestamp;
} fifoList;

fifoList *StoredTagLists;

void FilterTagList(tagInfo **tagList)
{
    long count = 0;

    fifoList *curr,
	     *prev;

    time_t now;

    misTrc(T_FLOW, "FilterTagList: Entering...");

    /*
     * Remove from the FIFO list the last stored tag list if there 
     * are more than a preconfigured value (default: 3 reads).
     */

    misTrc(T_FLOW, "FilterTagList: Removing excessive tag lists...");

    /* Set the current and previous pointers. */
    curr = StoredTagLists;
    prev = NULL;

    /* Get to the last stored tag list in the FIFO list. */
    while (curr && curr->next)
    {
	/* Increment the number of stored tag lists in the FIFO list. */
	count++;

	/* Keep a pointer to the previous stored tag list. */
	prev = curr;

	/* Move on to the next stored tag list. */
        curr = curr->next;
    }

    /* Remove this stored tag list if it puts us over the limit. */
    if (count == DEFAULT_KEEP_COUNT)
    {

        misTrc(T_FLOW, "FilterTagList: Removing excessive tag list...");

        /* Pull this stored tag list out of the FIFO list. */
	if (prev)
	    prev->next = curr->next;
        else
	    StoredTagLists = curr->next;

	/* Free resources associated with this stored tag list. */
        usaFreeTagList(curr);	
    }

    misTrc(T_FLOW, "FilterTagList: Removed excessive tag lists");

    /*
     * Remove from the FIFO list stored tag lists that are older
     * than a preconfigured value (default: 3 seconds).
     */

    misTrc(T_FLOW, "FilterTagList: Removing old tag lists...");

    /* Get the current time. */
    time(&now);

    /* Set the current and previous pointers. */
    curr = StoredTagLists;
    prev = NULL;

    /* Cycle through every stored tag list in the FIFO list. */
    while (curr)
    {
	/* Keep a pointer to the next stored tag list. */
	next = curr->next;

	/* Remove this stored tag list if it's too old. */
	if (curr->time < now + DEFAULT_KEEP_TIME)
	{
            misTrc(T_FLOW, "FilterTagList: Removing old tag list...");

            /* Pull this stored tag list out of the FIFO list. */
	    if (prev)
	        prev->next = curr->next;
            else
		StoredTagLists = curr->next;

	    /* Free resources associated with this stored tag list. */
            usaFreeTagList(curr);	
	}

	/* Move on to the next stored tag list. */
        curr = next;
    }

    misTrc(T_FLOW, "FilterTagList: Removed old tag lists");

    /*
     * Remove from the read tag list any tags that are in the 
     * FIFO list of stored tag lists.
     */

    misTrc(T_FLOW, "FilterTagList: Removing duplicate tags...");

    /* Cycle through each stored tag list in the list of stored tag lists. */
    for (storedTagList = StoredTagLists
	 storedTagList; 
	 storedTagList = storedTagList->next)
    {
	/* Cycle through each tag in the stored tag list. */
	for (storedTag = usaGetFirstTag(storedTagList->tagList)
	     storedTag; 
	     storedTag = usaGetNextTag(storedTag))
	{
	    /* Get the tag id of this stored tag. */
	    char *storedTagId = usaGetTagId(storedTag);

	    /* Cycle through each tag in the read tag list. */
	    for (readTag = usaGetFirstTag(readTagList); 
	         readTag; 
	         readTag = usaGetNextTag(readTag))
	    {
	        /* Get the tag id of this read tag. */
	        char *readTagId = usaGetTagId(readTag);

		/* Remove this tag if we've read it recently. */
	        if (strcmp(storedTagId, readTagId) == 0)
		{

		    misTrc(T_FLOW, "FilterTagList: Removing tag %s...", readTagId);

		    usaRemoveTag(readTagList, readTag);
		}
	    }
	}
    }

    misTrc(T_FLOW, "FilterTagList: Removed duplicate tags");

    misTrc(T_FLOW, "FilterTagList: Returning...");

    return;
} 

int main(int argc, char *argv[])
{
    exit(0);
}
