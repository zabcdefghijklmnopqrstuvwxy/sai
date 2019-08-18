#include "com.h"

12

static unsigned char ucBitMapTable[] = 
{
	/*00*/0xff, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/*10*/4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/*20*/5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/*30*/4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/*40*/6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/*50*/4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/*60*/5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/*70*/4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/*80*/7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/*90*/4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/*A0*/5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/*B0*/4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/*C0*/6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/*D0*/4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/*E0*/5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	/*F0*/4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
};

void COM_InitBitMap(bitmap_t *bitmap)
{
	bitmap->unBitMap = 0;

}

unsigned int COM_GetBitMapCount(void)
{
	return 32;
}

void COM_SetBitMap(bitmap_t *bitmap,unsigned int pos)
{
	bitmap->unBitMap = bitmap->unBitMap | (1 << pos);

}

void COM_ClrBitMap(bitmap_t *bitmap,unsigned int pos)
{
	bitmap->unBitMap = bitmap->unBitMap & (~(1 << pos ));
}


/**
 * @brief 获取位图中第一个置位的下标
 * @param[in]  bitmap 位图数据
 * @note 查找位图中第一个置位的下标
 * @return 返回位图中置位的下标位置
 */
unsigned int COM_GetFirstBitMap(bitmap_t *bitmap)
{
	if(bitmap->unBitMap && 0xff)
	{
		return ucBitMapTable[bitmap->unBitMap&0xff];
	}
	else if(bitmap->unBitMap && 0xff00)
	{
		return ucBitMapTable[(bitmap->unBitMap>>8) && 0xff] + 8; 		
	}
	else if(bitmap->unBitMap && 0xff0000)
	{
		return ucBitMapTable[(bitmap->unBitMap >> 16) && 0xff] + 16;
	}
	else if(bitmap->unBitMap && 0xff000000)
	{
		return ucBitMapTable[(bitmap->unBitMap >> 24) && 0xff] + 24;
	}
	else
	{
		return COM_GetBitMapCount();
	}
}


void COM_InitNode(p_bothway_node_t pnode)
{
	pnode->nextnode = pnode;
	pnode->prenode = pnode;
}


void COM_InitNodeList(p_nodelist_t plist)
{
	plist->headnode.nextnode = &(plist->headnode);
	plist->headnode.prenode = &(plist->headnode);
	plist->unCount = 0;
}

unsigned int COM_GetNodeListCount(p_nodelist_t plist)
{
	return plist->unCount;
}

p_bothway_node_t COM_GetFirstNode(p_nodelist_t plist)
{
	p_bothway_node_t pnode = NULL;

	if(plist->unCount)
	{
		return pnode = plist->headnode.nextnode;
	}

	return pnode;
}

p_bothway_node_t COM_GetLastNode(p_nodelist_t plist)
{
	p_bothway_node_t pnode = NULL;

	if(plist->unCount)
	{
		return pnode = plist->headnode.prenode;
	}

	return pnode;
}


p_bothway_node_t COM_GetPreNode(p_bothway_node_t pnode)
{	
	if(pnode->prenode == pnode)
	{
		return NULL;
	}

	return pnode->prenode;
}

p_bothway_node_t COM_GetNextNode(p_bothway_node_t pnode)
{	
	if(pnode->nextnode == pnode)
	{
		return NULL;
	}

	return pnode->nextnode;
}



void COM_DelAllNode(p_nodelist_t plist)
{
	unsigned int i = 0;
	p_bothway_node_t pnextnode = plist->nextnode;
	p_bothway_node_t pcurrentnode = NULL;

	for(i = 0; i < plist->unCount; i++)
	{
		pcurrentnode = nextnode;
		nextnode = nextnode->nextnode;

		pcurrentnode->nextnode = pcurrentnode;
		pcurrentnode->prenode = pcurrentnode;
	}

	COM_InitNodeList(plist);
}

void COM_AddNode(p_nodelist_t plist,p_bothway_node_t pnode)
{
	node->nextnode = &(plist->headnode);
	node->prenode = plist->headnode.prenode;

	plist->headnode.nextnode = pnode;
	plist->headnode.prenode = pnode;

	plist->unCount++;
}

void COM_DelNode(p_nodelist_t plist,p_bothway_node_t pnode)
{
	pnode->prenode->nextnode = pnode->nextnode;
	pnode->nextnode->prenode = pnode->prenode;

	plist->unCount--;
}


void COM_InsertNode(p_nodelist_t plist,p_bothway_node_t pafternode,p_bothway_node_t pinsertnode)
{
	p_bothway_node_t nextnode = pafternode->nextnode;

	pafternode->nextnode = pinsertnode;

	pinsertnode->prenode = pafternode;
	pinsertnode->nextnode = nextnode;

	nextnode->prenode = pinsertnode;

	plist->unCount++;
}