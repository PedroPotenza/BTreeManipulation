/* bt.h
header file for btree programs
*/
#define MAXKEYS 4 // ordem impar
#define MINKEYS MAXKEYS / 2
#define NIL (-1)
#define NOKEY '@'
#define NO 0
#define YES 1

typedef struct
{
    short keycount;           // number of keys in page
    char key[MAXKEYS];        // the actual keys
    short child[MAXKEYS + 1]; // ptrs to rrns of descendants
} BTPAGE;

#define PAGESIZE sizeof(btpage)

extern short root; // rrn of root page
extern int btfd;   // file descriptor of btree file
extern int infd;   // file descriptor of input file

/* prototypes */
btclose();
btopen();
btread(short rrn, BTPAGE *page_ptr);
btwrite(short rrn, BTPAGE *page_ptr);
create_root(char key, short left, short right);
short create_tree();
short getpage();
short getroot();
insert(short rrn, char key, short *promo_r_child, char *promo_key);
ins_in_page(char key, short r_child, BTPAGE *p_page);
pageinit(BTPAGE *p_page);
putroot(short root);
search_node(char key, BTPAGE *p_page, short *pos);
split(char key, short r_child, BTPAGE *p_oldpage, char *promo_key, short *promo_r_child, BTPAGE *p_newpage);

/* driver.c
Driver for btree tests
Open or creates b-tree file.
Get next key and calls insert to insert key in tree.
If necessary creates new root.
*/
#include <stdio.h>
//#include "bt.h"

int main()
{

    int promoted;   // boolean: tells if a promotion from below
    short root,     // rrn of root page
        promo_rrn;  // rrn promoted from below
    char promo_key, // key promoted from below
        key;        // next key to insert in tree

    if (btopen())
    {
        root = getroot();
    }
    else
    {
        root = create_tree();
    }
    while ((key = getchar()) != 'q')
    {
        promoted = insert(root, key, &promo_rrn, &promo_key);
        if (promoted)
            root = create_root(promo_key, root, promo_rrn);
    }
    btclose();
}

/* insert.c
Contains insert() function to insert a key into a btree.
Calls itself recursively until bottom of tree is reached.
Then insert key node.
If node is out of room,
- calls split() to split node
- promotes middle key and rrn of new node
*/
//#include "bt.h"
insert(short rrn, char key, short *promo_r_child, char *promo_key)
{
    BTPAGE page,         // current page
        newpage;         // new page created if split occurs
    int found, promoted; // boolean values
    short pos,
        p_b_rrn;  // rrn promoted from below
    char p_b_key; // key promoted from below

    if (rrn == NIL)
    {
        *promo_key = key;
        *promo_r_child = NIL;
        return (YES);
    }

    btread(rrn, &page);

    found = search_node(key, &page, &pos);

    if (found)
    {
        printf("Error: attempt to insert duplicate key: %c \n\007", key);

        return (0);
    }

    promoted = insert(page.child[pos], key, &p_b_rrn, &p_b_key);

    if (!promoted)
    {
        return (NO);
    }

    if (page.keycount < MAXKEYS) {
        
        ins_in_page(p_b_key, p_b_rrn, &page);
        btwrite(rrn, &page);
        return (NO);

    } else {

        split(p_b_key, p_b_rrn, &page, promo_key, promo_r_child, &newpage);
        btwrite(rrn, &page);
        btwrite(*promo_r_child, &newpage);
        return (YES);
    }
}