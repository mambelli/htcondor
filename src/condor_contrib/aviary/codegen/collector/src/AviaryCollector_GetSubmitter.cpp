

        /**
         * GetSubmitter.cpp
         *
         * This file was auto-generated from WSDL
         * by the Apache Axis2/C version: SNAPSHOT  Built on : Mar 10, 2008 (08:35:52 GMT+00:00)
         */
        
            #include "AviaryCollector_GetSubmitter.h"
          

       #ifdef __GNUC__
       # if __GNUC__ >= 4
       #pragma GCC diagnostic ignored "-Wcast-qual"
       #pragma GCC diagnostic ignored "-Wshadow"
       #pragma GCC diagnostic ignored "-Wunused-parameter"
       #pragma GCC diagnostic ignored "-Wunused-variable"
       #pragma GCC diagnostic ignored "-Wunused-value"
       #pragma GCC diagnostic ignored "-Wwrite-strings"
       #  if __GNUC_MINOR__ >= 6
       #pragma GCC diagnostic ignored "-Wenum-compare"
       #pragma GCC diagnostic ignored "-Wunused-but-set-variable"
       #pragma GCC diagnostic ignored "-Wunused-but-set-parameter"
       #  endif
       #  if __GNUC_MINOR__ >= 7
       #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
       #  endif
       # endif
       #endif
        
        #include <Environment.h>
        #include <WSFError.h>


        using namespace wso2wsf;
        using namespace std;
        
        using namespace AviaryCollector;
        
               /*
                * Implementation of the GetSubmitter|http://collector.aviary.grid.redhat.com Element
                */
           AviaryCollector::GetSubmitter::GetSubmitter()
        {

        
            qname = NULL;
        
                property_Ids  = NULL;
              
            isValidIds  = false;
        
                  qname =  axutil_qname_create (Environment::getEnv(),
                        "GetSubmitter",
                        "http://collector.aviary.grid.redhat.com",
                        NULL);
                
        }

       AviaryCollector::GetSubmitter::GetSubmitter(std::vector<wso2wsf::OMElement*>* arg_Ids)
        {
             
                   qname = NULL;
             
               property_Ids  = NULL;
             
            isValidIds  = true;
            
                 qname =  axutil_qname_create (Environment::getEnv(),
                       "GetSubmitter",
                       "http://collector.aviary.grid.redhat.com",
                       NULL);
               
                    property_Ids = arg_Ids;
            
        }
        AviaryCollector::GetSubmitter::~GetSubmitter()
        {
            resetAll();
        }

        bool WSF_CALL AviaryCollector::GetSubmitter::resetAll()
        {
            //calls reset method for all the properties owned by this method which are pointers.

            
             resetIds();//wso2wsf::OMElement*
          if(qname != NULL)
          {
            axutil_qname_free( qname, Environment::getEnv());
            qname = NULL;
          }
        
            return true;

        }

        

        bool WSF_CALL
        AviaryCollector::GetSubmitter::deserialize(axiom_node_t** dp_parent,bool *dp_is_early_node_valid, bool dont_care_minoccurs)
        {
          axiom_node_t *parent = *dp_parent;
          
          bool status = AXIS2_SUCCESS;
           
         const axis2_char_t* text_value = NULL;
         axutil_qname_t *mqname = NULL;
          
               int i = 0;
            
               int sequence_broken = 0;
               axiom_node_t *tmp_node = NULL;
            
            axutil_qname_t *element_qname = NULL; 
            
               axiom_node_t *first_node = NULL;
               bool is_early_node_valid = true;
               axiom_node_t *current_node = NULL;
               axiom_element_t *current_element = NULL;
            
              
              while(parent && axiom_node_get_node_type(parent, Environment::getEnv()) != AXIOM_ELEMENT)
              {
                  parent = axiom_node_get_next_sibling(parent, Environment::getEnv());
              }
              if (NULL == parent)
              {   
                return AXIS2_FAILURE;
              }
              

                    current_element = (axiom_element_t *)axiom_node_get_data_element(parent, Environment::getEnv());
                    mqname = axiom_element_get_qname(current_element, Environment::getEnv(), parent);
                    if (axutil_qname_equals(mqname, Environment::getEnv(), this->qname))
                    {
                        
                          first_node = axiom_node_get_first_child(parent, Environment::getEnv());
                          
                    }
                    else
                    {
                        WSF_LOG_ERROR_MSG(Environment::getEnv()->log, WSF_LOG_SI,
                              "Failed in building adb object for GetSubmitter : "
                              "Expected %s but returned %s",
                              axutil_qname_to_string(qname, Environment::getEnv()),
                              axutil_qname_to_string(mqname, Environment::getEnv()));
                        
                        return AXIS2_FAILURE;
                    }
                    
                       { 
                    /*
                     * building Ids array
                     */
                       std::vector<wso2wsf::OMElement*>* arr_list =new std::vector<wso2wsf::OMElement*>();
                   

                     
                     /*
                      * building ids element
                      */
                     
                     
                     
                                    element_qname = axutil_qname_create(Environment::getEnv(), "ids", NULL, NULL);
                                  
                               
                               for (i = 0, sequence_broken = 0, current_node = first_node; !sequence_broken && current_node != NULL;)
                                             
                               {
                                  if(axiom_node_get_node_type(current_node, Environment::getEnv()) != AXIOM_ELEMENT)
                                  {
                                     current_node =axiom_node_get_next_sibling(current_node, Environment::getEnv());
                                     is_early_node_valid = false;
                                     continue;
                                  }
                                  
                                  current_element = (axiom_element_t *)axiom_node_get_data_element(current_node, Environment::getEnv());
                                  mqname = axiom_element_get_qname(current_element, Environment::getEnv(), current_node);

                                  if (axutil_qname_equals(element_qname, Environment::getEnv(), mqname) || !axutil_strcmp("ids", axiom_element_get_localname(current_element, Environment::getEnv())))
                                  {
                                  
                                      is_early_node_valid = true;
                                      
                                     
                                          text_value = NULL; /* just to avoid warning */
                                          
                                              if(axiom_node_get_first_child(current_node, Environment::getEnv()))
                                              {
                                                  axiom_node_t *current_property_node = axiom_node_get_first_child(current_node, Environment::getEnv());
                                                  axiom_node_detach(current_property_node, Environment::getEnv());
                                                  arr_list->push_back(new OMElement(NULL,current_property_node));
                                              }
                                              else
                                              {
                                                  status = setIds(NULL);
                                              }
                                            
                                     if(AXIS2_FAILURE ==  status)
                                     {
                                         WSF_LOG_ERROR_MSG(Environment::getEnv()->log, WSF_LOG_SI, "failed in setting the value for ids ");
                                         if(element_qname)
                                         {
                                            axutil_qname_free(element_qname, Environment::getEnv());
                                         }
                                         if(arr_list)
                                         {
                                            delete arr_list;
                                         }
                                         return false;
                                     }

                                     i++;
                                    current_node = axiom_node_get_next_sibling(current_node, Environment::getEnv());
                                  }
                                  else
                                  {
                                      is_early_node_valid = false;
                                      sequence_broken = 1;
                                  }
                                  
                               }

                               
                                   if (i < 0)
                                   {
                                     /* found element out of order */
                                     WSF_LOG_ERROR_MSG( Environment::getEnv()->log,WSF_LOG_SI,"ids (@minOccurs = '0') only have %d elements", i);
                                     if(element_qname)
                                     {
                                        axutil_qname_free(element_qname, Environment::getEnv());
                                     }
                                     if(arr_list)
                                     {
                                        delete arr_list;
                                     }
                                     return false;
                                   }
                               

                               if(0 == arr_list->size())
                               {
                                    delete arr_list;
                               }
                               else
                               {
                                    status = setIds(arr_list);
                               }

                              
                            } 
                        
                  if(element_qname)
                  {
                     axutil_qname_free(element_qname, Environment::getEnv());
                     element_qname = NULL;
                  }
                 
          return status;
       }

          bool WSF_CALL
          AviaryCollector::GetSubmitter::isParticle()
          {
            
                 return false;
              
          }


          void WSF_CALL
          AviaryCollector::GetSubmitter::declareParentNamespaces(
                    axiom_element_t *parent_element,
                    axutil_hash_t *namespaces, int *next_ns_index)
          {
            
                  /* Here this is an empty function, Nothing to declare */
                 
          }

        
        
        axiom_node_t* WSF_CALL
	AviaryCollector::GetSubmitter::serialize(axiom_node_t *parent, 
			axiom_element_t *parent_element, 
			int parent_tag_closed, 
			axutil_hash_t *namespaces, 
			int *next_ns_index)
        {
            
            
         
         axiom_node_t *current_node = NULL;
         int tag_closed = 0;

         
         
                axiom_namespace_t *ns1 = NULL;

                axis2_char_t *qname_uri = NULL;
                axis2_char_t *qname_prefix = NULL;
                axis2_char_t *p_prefix = NULL;
            
               int i = 0;
               int count = 0;
               void *element = NULL;
             
                    axis2_char_t *text_value_1;
                    axis2_char_t *text_value_1_temp;
                    
               axis2_char_t *start_input_str = NULL;
               axis2_char_t *end_input_str = NULL;
               unsigned int start_input_str_len = 0;
               unsigned int end_input_str_len = 0;
            
            
               axiom_data_source_t *data_source = NULL;
               axutil_stream_t *stream = NULL;

             
                int next_ns_index_value = 0;
             
                    namespaces = axutil_hash_make(Environment::getEnv());
                    next_ns_index = &next_ns_index_value;
                     
                           ns1 = axiom_namespace_create (Environment::getEnv(),
                                             "http://collector.aviary.grid.redhat.com",
                                             "n"); 
                           axutil_hash_set(namespaces, "http://collector.aviary.grid.redhat.com", AXIS2_HASH_KEY_STRING, axutil_strdup(Environment::getEnv(), "n"));
                       
                     
                    parent_element = axiom_element_create (Environment::getEnv(), NULL, "GetSubmitter", ns1 , &parent);
                    
                    
                    axiom_element_set_namespace(parent_element, Environment::getEnv(), ns1, parent);


            
                    data_source = axiom_data_source_create(Environment::getEnv(), parent, &current_node);
                    stream = axiom_data_source_get_stream(data_source, Environment::getEnv());
                  
                       p_prefix = NULL;
                      

                   if (!isValidIds)
                   {
                      
                           /* no need to complain for minoccurs=0 element */
                            
                          
                   }
                   else
                   {
                     start_input_str = (axis2_char_t*)AXIS2_MALLOC(Environment::getEnv()->allocator, sizeof(axis2_char_t) *
                                 (4 + axutil_strlen(p_prefix) + 
                                  axutil_strlen("ids"))); 
                                 
                                 /* axutil_strlen("<:>") + 1 = 4 */
                     end_input_str = (axis2_char_t*)AXIS2_MALLOC(Environment::getEnv()->allocator, sizeof(axis2_char_t) *
                                 (5 + axutil_strlen(p_prefix) + axutil_strlen("ids")));
                                  /* axutil_strlen("</:>") + 1 = 5 */
                                  
                     

                   
                   
                     /*
                      * Parsing Ids array
                      */
                     if (property_Ids != NULL)
                     {
                        
                            sprintf(start_input_str, "<%s%sids>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":""); 
                            
                         start_input_str_len = axutil_strlen(start_input_str);

                         sprintf(end_input_str, "</%s%sids>",
                                 p_prefix?p_prefix:"",
                                 (p_prefix && axutil_strcmp(p_prefix, ""))?":":"");
                         end_input_str_len = axutil_strlen(end_input_str);

                         count = property_Ids->size();
                         for(i = 0; i < count; i++)
                         {
                            wso2wsf::OMElement* element = (*property_Ids)[i];

                            if(NULL == element) 
                            {
                                continue;
                            }

                    
                     
                     /*
                      * parsing ids element
                      */

                    
                    
                                std::string s = element->toString();
                                text_value_1 = (axis2_char_t*)(s.c_str());
                                
                                axutil_stream_write(stream, Environment::getEnv(), start_input_str, start_input_str_len);
                                
                                axutil_stream_write(stream, Environment::getEnv(), text_value_1, axutil_strlen(text_value_1));
                                
                                axutil_stream_write(stream, Environment::getEnv(), end_input_str, end_input_str_len);
                                
                         }
                     }
                   
                     
                     AXIS2_FREE(Environment::getEnv()->allocator,start_input_str);
                     AXIS2_FREE(Environment::getEnv()->allocator,end_input_str);
                 } 

                 
                   if(namespaces)
                   {
                       axutil_hash_index_t *hi;
                       void *val;
                       for (hi = axutil_hash_first(namespaces, Environment::getEnv()); hi; hi = axutil_hash_next(Environment::getEnv(), hi))
                       {
                           axutil_hash_this(hi, NULL, NULL, &val);
                           AXIS2_FREE(Environment::getEnv()->allocator, val);
                       }
                       axutil_hash_free(namespaces, Environment::getEnv());
                   }
                

            return parent;
        }


        

            /**
             * Getter for ids by  Property Number 1
             */
            std::vector<wso2wsf::OMElement*>* WSF_CALL
            AviaryCollector::GetSubmitter::getProperty1()
            {
                return getIds();
            }

            /**
             * getter for ids.
             */
            std::vector<wso2wsf::OMElement*>* WSF_CALL
            AviaryCollector::GetSubmitter::getIds()
             {
                return property_Ids;
             }

            /**
             * setter for ids
             */
            bool WSF_CALL
            AviaryCollector::GetSubmitter::setIds(
                    std::vector<wso2wsf::OMElement*>*  arg_Ids)
             {
                
                 int size = 0;
                 int i = 0;
                 bool non_nil_exists = false;
                

                if(isValidIds &&
                        arg_Ids == property_Ids)
                {
                    
                    return true;
                }

                
                 size = arg_Ids->size();
                 
                 if (size < 0)
                 {
                     WSF_LOG_ERROR_MSG( Environment::getEnv()->log,WSF_LOG_SI,"ids has less than minOccurs(0)");
                     return false;
                 }
                 for(i = 0; i < size; i ++ )
                 {
                     if(NULL != (*arg_Ids)[i])
                     {
                         non_nil_exists = true;
                         break;
                     }
                 }

                 

                
                resetIds();

                
                    if(NULL == arg_Ids)
                         
                {
                    /* We are already done */
                    return true;
                }
                
                        property_Ids = arg_Ids;
                        if(non_nil_exists)
                        {
                            isValidIds = true;
                        }
                        
                    
                return true;
             }

            
            /**
             * Get ith element of ids.
             */
            wso2wsf::OMElement* WSF_CALL
            AviaryCollector::GetSubmitter::getIdsAt(int i)
            {
                wso2wsf::OMElement* ret_val;
                if(property_Ids == NULL)
                {
                    return (wso2wsf::OMElement*)0;
                }
                ret_val =   (*property_Ids)[i];
                
                    return ret_val;
                  
            }

            /**
             * Set the ith element of ids.
             */
           bool WSF_CALL
            AviaryCollector::GetSubmitter::setIdsAt(int i,
                    wso2wsf::OMElement* arg_Ids)
            {
                 wso2wsf::OMElement* element;
                int size = 0;

                int non_nil_count;
                bool non_nil_exists = false;

                 

                if( isValidIds &&
                    property_Ids &&
                  
                    arg_Ids == (*property_Ids)[i])
                  
                 {
                    
                    return AXIS2_SUCCESS; 
                }

                   
                     non_nil_exists = true;
                  

                if(property_Ids == NULL)
                {
                    property_Ids = new std::vector<wso2wsf::OMElement*>();
                }
                else{
                /* check whether there already exist an element */
                element = (*property_Ids)[i];
                }

                
                        if(NULL != element)
                        {
                          
                          
                          
                                 // TODO Clear om Element
                                 delete element;
                             
                        }
                        
                    
                    if(!non_nil_exists)
                    {
                        
                        isValidIds = true;
                        (*property_Ids)[i]= NULL;
                        
                        return AXIS2_SUCCESS;
                    }
                
                    (*property_Ids)[i] = arg_Ids;
                  

               isValidIds = true;
                
                return AXIS2_SUCCESS;
            }

            /**
             * Add to ids.
             */
            bool WSF_CALL
            AviaryCollector::GetSubmitter::addIds(
                    wso2wsf::OMElement* arg_Ids)
             {

                
                    if( NULL == arg_Ids
                     )
                    {
                      
                           return true; 
                        
                    }
                  

                if(property_Ids == NULL)
                {
                    property_Ids = new std::vector<wso2wsf::OMElement*>();
                }
              
               property_Ids->push_back(arg_Ids);
              
                isValidIds = true;
                return true;
             }

            /**
             * Get the size of the ids array.
             */
            int WSF_CALL
            AviaryCollector::GetSubmitter::sizeofIds()
            {

                if(property_Ids == NULL)
                {
                    return 0;
                }
                return property_Ids->size();
            }

            /**
             * remove the ith element, same as set_nil_at.
             */
            bool WSF_CALL
            AviaryCollector::GetSubmitter::removeIdsAt(int i)
            {
                return setIdsNilAt(i);
            }

            

           /**
            * resetter for ids
            */
           bool WSF_CALL
           AviaryCollector::GetSubmitter::resetIds()
           {
               int i = 0;
               int count = 0;


               
                if (property_Ids != NULL)
                {
                  std::vector<wso2wsf::OMElement*>::iterator it =  property_Ids->begin();
                  for( ; it <  property_Ids->end() ; ++it)
                  {
                     wso2wsf::OMElement* element = *it;
                
            
                

                if(element != NULL)
                {
                   
                   
                        delete element;
                         element = NULL;
                     

                   }

                
                
                
               }

             }
                
                    if(NULL != property_Ids)
                 delete property_Ids;
                
               isValidIds = false; 
               return true;
           }

           /**
            * Check whether ids is nill
            */
           bool WSF_CALL
           AviaryCollector::GetSubmitter::isIdsNil()
           {
               return !isValidIds;
           }

           /**
            * Set ids to nill (currently the same as reset)
            */
           bool WSF_CALL
           AviaryCollector::GetSubmitter::setIdsNil()
           {
               return resetIds();
           }

           
           /**
            * Check whether ids is nill at i
            */
           bool WSF_CALL
           AviaryCollector::GetSubmitter::isIdsNilAt(int i)
           {
               return (isValidIds == false ||
                       NULL == property_Ids ||
                     NULL == (*property_Ids)[i]);
            }

           /**
            * Set ids to nil at i
            */
           bool WSF_CALL
           AviaryCollector::GetSubmitter::setIdsNilAt(int i)
           {
                int size = 0;
                int j;
                bool non_nil_exists = false;

                int k = 0;

                if(property_Ids == NULL ||
                            isValidIds == false)
                {
                    
                    non_nil_exists = false;
                }
                else
                {
                    size = property_Ids->size();
                    for(j = 0, k = 0; j < size; j ++ )
                    {
                        if(i == j) continue; 
                        if(NULL != (*property_Ids)[i])
                        {
                            k++;
                            non_nil_exists = true;
                            if( k >= 0)
                            {
                                break;
                            }
                        }
                    }
                }
                

                if( k < 0)
                {
                       WSF_LOG_ERROR_MSG(Environment::getEnv()->log, WSF_LOG_SI, "Size of the array of ids is beinng set to be smaller than the specificed number of minOccurs(0)");
                       return AXIS2_FAILURE;
                }
 
                if(property_Ids == NULL)
                {
                    isValidIds = false;
                    
                    return true;
                }
                 
                 /* check whether there already exist an element */
                 wso2wsf::OMElement* element = (*property_Ids)[i];
                if(NULL != element)
                {
                  
                  
                  
                     delete element;
                     
                 }
                 
                    if(!non_nil_exists)
                    {
                        
                        isValidIds = false;
                        (*property_Ids)[i] = NULL;
                        return AXIS2_SUCCESS;
                    }
                

                
                (*property_Ids)[i] = NULL;
                
                return AXIS2_SUCCESS;

           }

           
