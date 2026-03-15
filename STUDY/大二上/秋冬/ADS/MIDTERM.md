![[Pasted image 20251119171007.png]]
![[Pasted image 20251119171042.png]]
![[Pasted image 20251119173206.png]]
```
bool FindKey(BpTreeNode * const root, ElementType key){
    if (root == NULL) {
            return false;
    }
    int i = 0;
    BpTreeNode * node = root;
    while (!node->isLeaf) {
        i = 0;
        while (i < node->numKeys) {
            if (node->keys[i]<=key) i++;
            else break;
        }
        node = node->childrens[i];
    }
    for(i = 0; i < node->numKeys; i++){
        if(node->keys[i] == key)
            return true;
    }
    return false;
}
```