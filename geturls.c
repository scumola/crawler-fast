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

    LIBXML_TEST_VERSION

    doc = xmlReadFile(argv[1], NULL, XML_PARSE_RECOVER | XML_PARSE_NOERROR | XML_PARSE_NOWARNING);
//    doc = xmlReadFile(argv[1], NULL, 0);

    if (doc == NULL) {
//        printf("error: could not parse file %s\n", argv[1]);
	    exit(0);
    }

    root_element = xmlDocGetRootElement(doc);
    print_element_names(root_element);
    xmlFreeDoc(doc);
    xmlCleanupParser();
    return 0;
}
#else
int main(void) {
    fprintf(stderr, "Tree support not compiled in\n");
    exit(1);
}
#endif
