// Licensed under the MIT license. See LICENSE file in the project root for full license information.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "lib-util-c/sys_debug_shim.h"
#include "lib-util-c/binary_tree.h"
#include "lib-util-c/app_logging.h"

#define USE_RECURSION
#define NUM_OF_CHARS    8
static const char LEFT_PARENTHESIS = '(';
static const char RIGHT_PARENTHESIS = ')';


typedef struct NODE_INFO_TAG
{
    NODE_KEY key;
    void* data;
    struct NODE_INFO_TAG* parent;
    struct NODE_INFO_TAG* right;
    struct NODE_INFO_TAG* left;
    // Keeps track of the child nodes balance
    // child on right = -1
    // child on left = +1
    int balance_factor;
    size_t height;
} NODE_INFO;

typedef struct BINARY_TREE_INFO_TAG
{
    size_t items;
    size_t height;
    NODE_INFO* root_node;
} BINARY_TREE_INFO;

static int construct_visual_representation(const NODE_INFO* node_info, char* visualization, size_t pos)
{
    /*
            10
           /  \
          5    15
         / \   / \
        3   7 11  18
        10(5(3)(7))(15(11)(18)))
    */
    // [1, 2, 3, 4]
    // 1(2(4))(3)

    if (node_info != NULL)
    {
        char temp[8];
        int len = sprintf(temp, "%x", node_info->key);
        memcpy(visualization+pos, temp, len);
        pos += len;
        if (node_info->left != NULL)
        {
            memcpy(visualization + pos, &LEFT_PARENTHESIS, 1);
            pos += 1;
            pos = construct_visual_representation(node_info->left, visualization, pos);
            memcpy(visualization + pos, &RIGHT_PARENTHESIS, 1);
            pos += 1;
        }
        if (node_info->right != NULL)
        {
            memcpy(visualization + pos, &LEFT_PARENTHESIS, 1);
            pos += 1;
            pos = construct_visual_representation(node_info->right, visualization, pos);
            memcpy(visualization + pos, &RIGHT_PARENTHESIS, 1);
            pos += 1;
        }
    }
    return pos;
}

static int calculate_balance_factor(const NODE_INFO* node_info)
{
    int result;
    result = node_info->left == NULL ? 0 : node_info->left->height;
    result -= node_info->right == NULL ? 0 : node_info->right->height;
    return result;
}

static NODE_INFO* create_new_node(NODE_KEY key_value, void* data)
{
    NODE_INFO* result;
    if ((result = (NODE_INFO*)malloc(sizeof(NODE_INFO))) == NULL)
    {
        log_error("Failure allocating tree node");
    }
    else
    {
        memset(result, 0, sizeof(NODE_INFO));
        result->key = key_value;
        result->data = data;
    }
    return result;
}

static void print_tree(const NODE_INFO* node_info, size_t indent_level)
{
    if (node_info != NULL)
    {
        for (size_t index = 0; index < indent_level; index++)
            printf("\t");
        printf("%d\n", node_info->key);
        print_tree(node_info->left, indent_level + 1);
        print_tree(node_info->right, indent_level + 1);
    }
}

static void rotate_right(NODE_INFO* node_info)
{
    NODE_INFO* tpm_node = node_info;

    node_info->parent->left = node_info->left;
    node_info->parent->left->balance_factor = 0;
    node_info->parent->left->parent = node_info->parent;
    node_info->parent->balance_factor++;
    if (node_info->parent->left->right == NULL)
    {
        node_info->parent->left->right = tpm_node;
        tpm_node->parent = node_info->parent->left;
        tpm_node->left = tpm_node->right = NULL;
        tpm_node->balance_factor = 0;
    }
    else
    {
        tpm_node->left = node_info->parent->left->right;
        tpm_node->right = NULL;
        node_info->parent->left->right = tpm_node;
        tpm_node->balance_factor = -1;
    }
}

static void rotate_left(NODE_INFO* node_info)
{
    NODE_INFO* tpm_node = node_info;

    node_info->parent->right = node_info->right;
    // The current node gets moved down
    node_info->height--;

    node_info->parent->right->balance_factor = 0;
    node_info->parent->right->parent = node_info->parent;
    node_info->parent->balance_factor++;
    tpm_node->parent = node_info->parent->right;
    if (node_info->parent->right->left == NULL)
    {
        node_info->parent->left = tpm_node;
        tpm_node->right = tpm_node->left = NULL;
        tpm_node->balance_factor = 0;
        tpm_node->height = tpm_node->parent->height-1;
    }
    else
    {
        tpm_node->right = node_info->parent->left->right;
        tpm_node->left = NULL;
        tpm_node->height = tpm_node->parent->height - 1;
        node_info->parent->right->left = tpm_node;
        tpm_node->balance_factor = 1;
    }
}

static int rebalance_if_neccessary(NODE_INFO* node_info)
{
    int result;
    if (node_info->balance_factor < -1 && node_info->right->balance_factor == -1)
    {
        rotate_left(node_info);
        result = 1;
        log_debug("rotate right");
    }
    else if (node_info->balance_factor > 1 && node_info->left->balance_factor == 1)
    {
        rotate_right(node_info);
        result = -1;
        log_debug("rotate left");
    }
    else if (node_info->balance_factor < -1 && node_info->right->balance_factor == 1)
    {
        // Do Left right rotation
        // Make the left child, a child of the left's right node
        node_info->right->left->right = node_info->right;
        node_info->right->left->right->height--;
        node_info->right->parent = node_info->right->left;
        node_info->right->left->parent = node_info;
        node_info->right = node_info->right->left;
        node_info->right->height++;
        node_info->right->right->left = NULL;
        // Do the left rotation
        rotate_left(node_info);
        result = 1;
    }
    else if (node_info->balance_factor > 1 && node_info->left->balance_factor == -1)
    {
        // Do right left rotation
        node_info->left->right->left = node_info->left;
        node_info->left->right->left->height--;
        node_info->left->parent = node_info->left->right;
        node_info->left->right->parent = node_info;
        node_info->left = node_info->left->right;
        node_info->left->height++;
        node_info->left->left->right = NULL;
        // Now do a right rotation
        rotate_right(node_info);
        result = -1;
    }
    else
    {
        result = 0;
    }
    return result;
}

static int compare_node_values(const NODE_KEY* value_1, const NODE_KEY* value_2)
{
    if (*value_1 > *value_2) return 1;
    else if (*value_1 < *value_2) return -1;
    else return 0;
}

static NODE_INFO* find_node(NODE_INFO* node_info, const NODE_KEY* value)
{
    NODE_INFO* result;
    if (node_info == NULL)
    {
        result = NULL;
    }
    else
    {
#ifdef USE_RECURSION
        int compare_value = compare_node_values(&node_info->key, value);
        if (compare_value > 0)
        {
            result = find_node(node_info->left, value);
        }
        else if (compare_value < 0)
        {
            result = find_node(node_info->right, value);
        }
        else
        {
            result = node_info;
        }
#else
        int compare_value;
        NODE_INFO* compare_node = node_info;

        result = NULL;
        while (compare_node != NULL)
        {
            compare_value = compare_node_values(&compare_node->key, value);
            if (compare_value > 0)
            {
                compare_node = compare_node->left;
            }
            else if (compare_value < 0)
            {
                compare_node = compare_node->right;
            }
            else
            {
                result = compare_node;
            }
        }
#endif
    }
    return result;
}

typedef enum INSERT_NODE_TYPE_TAG
{
    INSERT_NODE_INSERTED,
    INSERT_NODE_NO_OP,
    INSERT_NODE_REBALANCE,
    INSERT_NODE_FAILED
} INSERT_NODE_TYPE;

static INSERT_NODE_TYPE insert_into_tree(NODE_INFO** target_node, NODE_INFO* new_node)
{
    INSERT_NODE_TYPE result = INSERT_NODE_FAILED;
#ifdef USE_RECURSION
    if (*target_node == NULL)
    {
        *target_node = new_node;
        (*target_node)->height = 1;
        result = INSERT_NODE_INSERTED;
    }
    else if (new_node->key > (*target_node)->key)
    {
        if ((result = insert_into_tree(&(*target_node)->right, new_node)) != INSERT_NODE_FAILED)
        {
            // Increment the height
            if ((*target_node)->height <= (*target_node)->right->height || result == INSERT_NODE_REBALANCE)
            {
                // Or rebalance the height changes should reverberate up the tree
                (*target_node)->height = (*target_node)->right->height + 1;
            }
            // If you inserted a direct child increment
            // the balance factor
            if (result == INSERT_NODE_INSERTED)
            {
                // Set parent node
                (*target_node)->right->parent = *target_node;

                // Only return inserted for
                result = INSERT_NODE_NO_OP;
            }
        }
    }
    else if (new_node->key < (*target_node)->key)
    {
        if ((result = insert_into_tree(&(*target_node)->left, new_node)) != INSERT_NODE_FAILED)
        {
            // Increment the height
            if ((*target_node)->height <= (*target_node)->left->height)
            {
                (*target_node)->height = (*target_node)->left->height + 1;
            }

            // If you inserted a direct child increment
            // the balance factor
            if (result == INSERT_NODE_INSERTED)
            {
                // Set parent node
                (*target_node)->left->parent = *target_node;

                // Only return inserted for
                result = INSERT_NODE_NO_OP;
            }
        }
    }
#else
    int continue_run = 1;
    result = INSERT_NODE_INSERTED;
    if (*target_node == NULL)
    {
        *target_node = new_node;
        (*target_node)->height++;
    }
    else
    {
        NODE_INFO* increment_node = NULL;
        NODE_INFO* prev_node = NULL;
        NODE_INFO* current_node = prev_node = *target_node;
        do
        {
            if (new_node->key > current_node->key)
            {
                if (current_node->right == NULL)
                {
                    current_node->right = new_node;
                    current_node->right->parent = current_node;
                    current_node->right->height = 1;
                    continue_run = 0;
                    // If the current_node->left is NULL then increment height
                    if (current_node->left == NULL)
                    {
                        increment_node = current_node->right;
                    }
                }
                else
                {
                    prev_node = current_node;
                    current_node = current_node->right;
                }
            }
            else if (new_node->key < current_node->key)
            {
                if (current_node->left == NULL)
                {
                    current_node->left = new_node;
                    current_node->left->parent = current_node;
                    current_node->left->height = 1;
                    continue_run = 0;
                }
                else
                {
                    prev_node = current_node;
                    current_node = current_node->left;
                }
                // If the current_node->left is NULL then increment height
                if (current_node->right == NULL)
                {
                    increment_node = current_node->left;
                }
            }
            else
            {
                // Failure
                continue_run = 0;
                result = INSERT_NODE_FAILED;
            }
        } while (continue_run != 0);


        if (increment_node != NULL)
        {
            NODE_INFO* parent_node = increment_node->parent;
            // Go up the tree and increment height of parents
            while (parent_node != NULL)
            {
                parent_node->height++;
                parent_node = parent_node->parent;
            }
        }
    }

#endif
    if (result != INSERT_NODE_FAILED)
    {
        // If a decendant has been added then calculate
        // the balance factor by addition
        (*target_node)->balance_factor = calculate_balance_factor(*target_node);

        if (rebalance_if_neccessary(*target_node) != 0)
        {
            result = INSERT_NODE_REBALANCE;
        }
    }
    return result;
}

static int remove_node(NODE_INFO** root_node, const NODE_KEY* node_key, tree_remove_callback remove_callback)
{
    int result;
    NODE_INFO* node_info = *root_node;
    NODE_INFO* current_node = find_node(node_info, node_key);
    if (current_node == NULL)
    {
        result = __LINE__;
    }
    else
    {
        NODE_INFO* previous_node = current_node->parent;

        if (remove_callback != NULL)
        {
            remove_callback(current_node->data);
        }

        // Remove the node
        result = 0;
        // Does the node have a single child
        if ((current_node->left != NULL && current_node->right == NULL) || (current_node->right != NULL && current_node->left == NULL))
        {
            // The right node is there
            if (current_node->left == NULL && current_node->right != NULL)
            {
                // If previous_node's left tree equals Node n
                if (previous_node->left == current_node)
                {
                    // then predecessor's left tree becomes n's right tree
                    // and delete n
                    previous_node->left = current_node->right;
                    current_node->right->parent = previous_node;
                    free(current_node);
                    current_node = NULL;
                    previous_node->balance_factor--;
                }
                // If predecessor's right tree equals Node n
                else
                {
                    // then predecessor's right tree becomes n's right tree
                    // and delete n
                    previous_node->right = current_node->right;
                    current_node->right->parent = previous_node;
                    free(current_node);
                    current_node = NULL;
                    previous_node->balance_factor++;
                }
            }
            else // Left node Present, No Right node Present
            {
                if (previous_node->left == current_node)
                {
                    previous_node->left = current_node->left;
                    free(current_node);
                    current_node = NULL;
                    previous_node->balance_factor--;
                }
                else
                {
                    previous_node->right = current_node->left;
                    current_node->left->parent = previous_node;
                    free(current_node);
                    current_node = NULL;
                    previous_node->balance_factor++;
                }
            }
        }
        else if (current_node->left == NULL && current_node->right == NULL)
        {
            if (previous_node->left == current_node)
            {
                previous_node->left = NULL;
                previous_node->balance_factor--;
            }
            else
            {
                previous_node->right = NULL;
                previous_node->balance_factor++;
            }
            free(current_node);
        }
        // CASE 3: Node has two children
        // Replace Node with smallest value in right subtree
        else if (current_node->left != NULL && current_node->right != NULL)
        {
            // If the node's right child has a left child
            // Move all the way down left to locate smallest element
            if ((current_node->right)->left != NULL)
            {
                NODE_INFO* left_current;
                NODE_INFO* left_current_prev;
                left_current_prev = current_node->right;
                left_current = (current_node->right)->left;
                while (left_current->left != NULL)
                {
                    left_current_prev = left_current;
                    left_current = left_current->left;
                }
                current_node->data = left_current->data;
                free(left_current);
                left_current_prev->left = NULL;
            }
            else
            {
                NODE_INFO* temp = current_node;
                if (temp->parent == NULL)
                {
                    // deleting the root
                    // Find the min key in the right subtree
                    NODE_INFO* min_node;
                    NODE_INFO* observe_node = min_node = temp->right->left;
                    if (observe_node == NULL)
                    {
                        min_node = temp->right;
                    }
                    else
                    {
                        do
                        {
                            if (observe_node->key < min_node->key)
                            {
                                min_node = observe_node;
                            }
                            observe_node = observe_node->left;
                        } while (observe_node != NULL);
                    }
                    if (min_node->right != NULL)
                    {
                        // Exchange the min_node->right with min_node
                        if (min_node->parent != current_node)
                        {
                            min_node->parent->left = min_node->right;
                        }
                        min_node->right->parent = min_node->parent;
                        min_node->right->height = min_node->height;
                    }
                    else if (min_node->parent != current_node && min_node->parent->left != NULL)
                    {
                        min_node->parent->left = NULL;
                    }
                    min_node->parent = NULL;
                    min_node->height = current_node->height;
                    if (current_node->right != NULL && min_node != current_node->right)
                    {
                        min_node->right = current_node->right;
                        current_node->right->parent = min_node;
                    }
                    if (current_node->left != NULL && min_node != current_node->left)
                    {
                        min_node->left = current_node->left;
                        current_node->left->parent = min_node;
                    }
                    *root_node = min_node;
                    free(current_node);
                }
                else
                {
                    // Find the parent node that points to this
                    // and change it
                    if (temp->parent->right == temp)
                    {
                        temp->parent->right = temp->right;
                        temp->parent->balance_factor--;
                    }
                    else
                    {
                        temp->parent->left = temp->right;
                        temp->parent->balance_factor++;
                    }
                    current_node->right->parent = temp->parent;
                    current_node->right->left = temp->left;
                    current_node = temp->right;
                    current_node->balance_factor--;
                    //current_node->parent = current_node->parent;
                    free(temp);
                }
            }
        }
    }
    return result;
}

static void clear_tree(NODE_INFO* node_info)
{
#ifdef USE_RECURSION
    // Clear right
    if (node_info->right != NULL)
    {
        clear_tree(node_info->right);
        free(node_info->right);
    }
    // Clear left
    if (node_info->left != NULL)
    {
        clear_tree(node_info->left);
        free(node_info->left);
    }
#else
    NODE_INFO* target_node = node_info;
    NODE_INFO* previous_node = NULL;

    while (target_node != NULL)
    {
        if (target_node->left == NULL)
        {
            target_node = target_node->right;
        }
        else
        {
            // Find the inorder predecessor of current
            previous_node = target_node->left;
            while (previous_node->right != NULL && previous_node->right != target_node)
            {
                previous_node = previous_node->right;
            }

            // Make current as right child of its inorder predecessor
            if (previous_node->right == NULL)
            {
                previous_node->right = target_node;
                target_node = target_node->left;
            }
            // Revert the changes made in if part to restore the original
            // tree i.e., fix the right child of predecssor
            else
            {
                previous_node->right = NULL;
                target_node = target_node->right;
            }
        }
    }
#endif
}

BINARY_TREE_HANDLE binary_tree_create()
{
    BINARY_TREE_INFO* result = (BINARY_TREE_INFO*)malloc(sizeof(BINARY_TREE_INFO));
    if (result == NULL)
    {
        log_error("FAILURE: unable to allocate Binary tree info");
        result = NULL;
    }
    else
    {
        memset(result, 0, sizeof(BINARY_TREE_INFO));
    }
    return result;
}

void binary_tree_destroy(BINARY_TREE_HANDLE handle)
{
    if (handle != NULL)
    {
        if (handle->root_node != NULL)
        {
            clear_tree(handle->root_node);
            free(handle->root_node);
        }
        free(handle);
    }
}

int binary_tree_insert(BINARY_TREE_HANDLE handle, NODE_KEY value, void* data)
{
    int result;
    if (handle == NULL)
    {
        log_error("FAILURE: Invalid handle specified on insert");
        result = __LINE__;
    }
    else
    {
        size_t current_height = 0;
        NODE_INFO* new_node = create_new_node(value, data);
        if (new_node == NULL)
        {
            log_error("FAILURE: Creating new node on insert");
            result = __LINE__;
        }
        else if (insert_into_tree(&handle->root_node, new_node) == INSERT_NODE_FAILED)
        {
            log_error("FAILURE: Inserting new node");
            free(new_node);
            result = __LINE__;
        }
        else
        {
            handle->height = current_height;
            handle->items++;
            result = 0;
        }
    }
    return result;
}

int binary_tree_remove(BINARY_TREE_HANDLE handle, NODE_KEY value, tree_remove_callback remove_callback)
{
    (void)value;
    int result;
    if (handle == NULL)
    {
        log_error("FAILURE: Invalid handle specified on remove");
        result = __LINE__;
    }
    else
    {
        result = remove_node(&handle->root_node, &value, remove_callback);
        if (result == 0)
        {
            handle->items-- ;
        }
    }
    return result;
}

void* binary_tree_find(BINARY_TREE_HANDLE handle, NODE_KEY find_value)
{
    void* result;
    if (handle == NULL)
    {
        log_error("FAILURE: Invalid handle specified on find");
        result = NULL;
    }
    else
    {
        const NODE_INFO* node_info = find_node(handle->root_node, &find_value);
        if (node_info == NULL)
        {
            log_debug("Item Not found");
            result = NULL;
        }
        else
        {
            result = node_info->data;
        }
    }
    return result;
}

size_t binary_tree_item_count(BINARY_TREE_HANDLE handle)
{
    size_t result;
    if (handle == NULL)
    {
        log_error("FAILURE: Invalid handle specified on remove");
        result = __LINE__;
    }
    else
    {
        result = handle->items;
    }
    return result;
}

size_t binary_tree_height(BINARY_TREE_HANDLE handle)
{
    size_t result;
    if (handle == NULL)
    {
        log_error("FAILURE: Invalid handle specified on remove");
        result = __LINE__;
    }
    else
    {
        result = handle->root_node->height;
    }
    return result;
}

void binary_tree_print(BINARY_TREE_HANDLE handle)
{
    if (handle == NULL)
    {
        log_error("FAILURE: Invalid handle specified on print");
    }
    else
    {
        print_tree(handle->root_node, 0);
    }
}

char* binary_tree_construct_visual(BINARY_TREE_HANDLE handle)
{
    char* result;
    if (handle == NULL)
    {
        log_error("FAILURE: Invalid handle specified on construct visual");
        result = NULL;
    }
    else
    {
        // Allocate the result
        if (handle->items > 0)
        {
            size_t len = (handle->items*NUM_OF_CHARS) + (handle->items * 2);
            result = (char*)malloc(len + 1);
            memset(result, 0, len + 1);
            construct_visual_representation(handle->root_node, result, 0);
        }
        else
        {
            result = (char*)malloc(1);
            result[0] = '\0';
        }
    }
    return result;
}