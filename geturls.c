/**
 * section: Tree
 * synopsis: Navigates a tree to print element names
 * purpose: Parse a file to a tree, use xmlDocGetRootElement() to
 *          get the root element, then walk the document and print
 *          all the element name in document order.
 * usage: tree1 filename_or_URL
 * test: tree1 test2.xml > tree1.tmp ; diff tree1.tmp tree1.res ; rm tree1.tmp
 * author: Dodji Seketeli
 * copy: see Copyright for the status of this software.
 */
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#ifdef LIBXML_TREE_ENABLED

static void
print_element_names(xmlNode * a_node)
{
    xmlNode *cur_node = NULL;
    xmlAttrPtr attr = NULL;

    for (cur_node = a_node; cur_node; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {

	    if(xmlStrcasecmp(cur_node->name, (const xmlChar*)"A") == 0)
            {
                for(attr = cur_node->properties; attr != NULL; attr = attr->next)
                {
                    if(xmlStrcasecmp(attr->name, (const xmlChar*)"HREF") == 0)
                    {
                        printf("%s\n", attr->children->content);
                    }
                }
            }

//	    if (strcmp(cur_node->name,"a") == 0) {
//            	printf("node type: Element, name: %s\n", cur_node->name);
//	    }
        }

        print_element_names(cur_node->children);
    }
}

int
main(int argc, char **argv)
{
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    if (argc != 2)
        return(1);

    /*
     * this initialize the library and check potential ABI mismatches
     * between the version it was compiled for and the actual shared
     * library used.
     */
    LIBXML_TEST_VERSION

    /*parse the file and get the DOM */
    doc = xmlReadFile(argv[1], NULL, XML_PARSE_RECOVER | XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
//    doc = xmlReadFile(argv[1], NULL, 0);

    if (doc == NULL) {
//        printf("error: could not parse file %s\n", argv[1]);
	exit(0);
    }

    /*Get the root element node */
    root_element = xmlDocGetRootElement(doc);

    print_element_names(root_element);

    /*free the document */
    xmlFreeDoc(doc);

    /*
     *Free the global variables that may
     *have been allocated by the parser.
     */
    xmlCleanupParser();

    return 0;
}
#else
int main(void) {
    fprintf(stderr, "Tree support not compiled in\n");
    exit(1);
}
#endif
