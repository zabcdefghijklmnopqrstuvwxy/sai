#ifndef _COM_H_
#define	_COM_H_

#define			NODEPARENT(node,parent,name)		(parent*)((unsigned int)node - (unsigned int)&((parent*)0)->name)


typedef struct 
{
	unsigned int unBitMap;
}bitmap_t;


typedef struct
{
	struct p_bothway_node_t prenode;
	struct p_bothway_node_t nextnode;
}bothway_node_t,*p_bothway_node_t;


typedef struct 
{
	bothway_node_t headnode;
	unsigned int unCount;
}nodelist_t,*p_nodelist_t;


void COM_InitBitMap(bitmap_t *bitmap);

unsigned int COM_GetBitMapCount(void);

void COM_SetBitMap(bitmap_t *bitmap,unsigned int pos);

void COM_ClrBitMap(bitmap_t *bitmap,unsigned int pos);

unsigned int COM_GetFirstBitMap(bitmap_t *bitmap);


void COM_InitNodeList(p_nodelist_t plist);

p_bothway_node_t COM_GetFirstNode(p_nodelist_t plist);

p_bothway_node_t COM_GetLastNode(p_nodelist_t plist);

p_bothway_node_t COM_GetPreNode(p_bothway_node_t pnode);

p_bothway_node_t COM_GetNextNode(p_bothway_node_t pnode);

void COM_DelAllNode(p_nodelist_t plist);

void COM_AddNode(p_nodelist_t plist,p_bothway_node_t pnode);

void COM_DelNode(p_nodelist_t plist,p_bothway_node_t pnode);

void COM_InsertNode(p_nodelist_t plist,p_bothway_node_t pafternode,p_bothway_node_t pinsertnode);

#endif