#include <Core/XMLDom.h>
#include <Core/Utility.h>
#include <Core/Exception.h>
#include <IO/Stream.h>

namespace RcEngine{


rapidxml::node_type MapToRapidxml(XMLNodeType type)
{
	switch(type)
	{
	case XML_Node_Document:
		return rapidxml::node_document;
	case XML_Node_Element:
		return rapidxml::node_element;
	case XML_Node_Data:
		return rapidxml::node_data;
	case XML_Node_CData:
		return rapidxml::node_cdata;
	case XML_Node_Comment:
		return rapidxml::node_comment;
	case XML_Node_Declaration:
		return rapidxml::node_declaration;
	case XML_Node_Doctype:
		return rapidxml::node_doctype;
	case XML_Node_PI:
		return rapidxml::node_pi;
	}

	return rapidxml::node_pi;
}

XMLNodeType UnMapToRapidxml(rapidxml::node_type type)
{
	switch(type)
	{
	case rapidxml::node_document:
		return XML_Node_Document;
	case rapidxml::node_element:
		return XML_Node_Element;
	case rapidxml::node_data:
		return XML_Node_Data;
	case rapidxml::node_cdata:
		return XML_Node_CData;
	case rapidxml::node_comment:
		return XML_Node_Comment;
	case rapidxml::node_declaration:
		return XML_Node_Declaration;
	case rapidxml::node_doctype:
		return XML_Node_Doctype;
	case rapidxml::node_pi:
		return XML_Node_PI;
	}

	return XML_Node_PI;
}


XMLDoc::XMLDoc()
{

}

XMLNodePtr XMLDoc::Parse( Stream& source )
{
	uint32_t size = source.GetSize();
	mXMLSrc.resize(size + 1, 0);
	source.Read(&mXMLSrc[0], size);
	mXMLSrc[size] = '\0';

	mDocument.parse<0>(&mXMLSrc[0]);
	mRoot = std::make_shared<XMLNode>(mDocument.first_node());

	return mRoot;
}

void XMLDoc::Print( std::ostream& os )
{
	//os << "<?xml version=\"1.0\"?>" << std::endl; 
	os << mDocument;
}

XMLNodePtr XMLDoc::AllocateNode( XMLNodeType type, const std::string& name )
{
	rapidxml::xml_node<>* node = mDocument.allocate_node(MapToRapidxml(type),
		mDocument.allocate_string(name.c_str(), name.length()), 0, name.length());

	return std::make_shared<XMLNode>(node);
}

XMLAttributePtr XMLDoc::AllocateAttributeInt( const std::string& name, int32_t value )
{
	return AllocateAttributeString(name, LexicalCast<std::string>(value));
}

XMLAttributePtr XMLDoc::AllocateAttributeUInt(const std::string& name, uint32_t value)
{
	return AllocateAttributeString(name, LexicalCast<std::string>(value));
}

XMLAttributePtr XMLDoc::AllocateAttributeFloat(const std::string& name, float value)
{
	return AllocateAttributeString(name, LexicalCast<std::string>(value));
}

XMLAttributePtr XMLDoc::AllocateAttributeString(const std::string& name, const std::string& value)
{
	/*rapidxml::xml_attribute<>* attr = mDocument.allocate_attribute(
	mDocument.allocate_string(name.c_str(), name.length()),
	mDocument.allocate_string(value.c_str(), value.length()));*/
	
	const char* attrName = mDocument.allocate_string(name.c_str(), name.length());
	const char* attrValue = mDocument.allocate_string(value.c_str(), value.length());

	rapidxml::xml_attribute<>* attr = mDocument.allocate_attribute(attrName, attrValue, name.length(), value.length());

	return std::make_shared<XMLAttribute>(attr);
}

void XMLDoc::RootNode( const XMLNodePtr& newNode )
{
	mDocument.remove_all_nodes();
	mDocument.append_node(newNode->mNode);
	mRoot = newNode;
}

XMLNode::XMLNode( rapidxml::xml_node<>* node )
	: mNode(node)
{
}

std::string XMLNode::NodeName() const
{
	assert(mNode);
	return std::string(mNode->name(), mNode->name_size());
}

XMLNodeType XMLNode::NodeType() const
{
	return UnMapToRapidxml(mNode->type());
}

XMLAttributePtr XMLNode::FirstAttribute( const std::string& name  )
{
	rapidxml::xml_attribute<> *attr = mNode->first_attribute(name.c_str());
	if(attr)
	{
		return std::make_shared<XMLAttribute>(attr);
	}
	else
	{
		return nullptr;
	}
}

XMLAttributePtr XMLNode::FirstAttribute()
{
	rapidxml::xml_attribute<> *attr = mNode->first_attribute();
	if(attr)
	{
		return std::make_shared<XMLAttribute>(attr);
	}
	else
	{
		return nullptr;
	}
}

XMLAttributePtr XMLNode::LastAttribute( const std::string& name )
{
	rapidxml::xml_attribute<>* attr = mNode->last_attribute(name.c_str());
	if(attr)
	{
		return std::make_shared<XMLAttribute>(attr);
	}
	else
	{
		return nullptr;
	}
}

XMLAttributePtr XMLNode::LastAttribute()
{
	rapidxml::xml_attribute<>* attr = mNode->last_attribute();
	if(attr)
	{
		return std::make_shared<XMLAttribute>(attr);
	}
	else
	{
		return nullptr;
	}
}

XMLAttributePtr XMLNode::Attribute( const std::string& name )
{
	return FirstAttribute(name);
}

int32_t XMLNode::AttributeInt( const std::string& name , int32_t defaultVar  )
{
	XMLAttributePtr attr = Attribute(name);
	return attr ? attr->ValueInt() : defaultVar;
}

uint32_t XMLNode::AttributeUInt( const std::string& name , uint32_t defaultVar  )
{
	XMLAttributePtr attr = Attribute(name);
	return attr ? attr->ValueUInt() : defaultVar;
}

float XMLNode::AttributeFloat( const std::string& name , float defaultVar )
{
	XMLAttributePtr attr = Attribute(name);
	return attr ? attr->ValueFloat() : defaultVar;
}

std::string XMLNode::AttributeString( const std::string& name , std::string defaultVar )
{
	XMLAttributePtr attr = Attribute(name);
	return attr ? attr->ValueString() : defaultVar;
}

XMLNodePtr XMLNode::FirstNode( const std::string& name )
{
	rapidxml::xml_node<>* node = mNode->first_node(name.c_str());
	if(node){
		return std::make_shared<XMLNode>(node);
	}
	else{
		return nullptr;
	}
}

XMLNodePtr XMLNode::LastNode( const std::string& name )
{
	rapidxml::xml_node<>* node = mNode->last_node(name.c_str());
	if(node){
		return std::make_shared<XMLNode>(node);
	}
	else{
		return nullptr;
	}
}

XMLNodePtr XMLNode::FirstNode()
{
	rapidxml::xml_node<>* node = mNode->first_node();
	if(node){
		return std::make_shared<XMLNode>(node);
	}
	else{
		return nullptr;
	}
}

XMLNodePtr XMLNode::LastNode()
{
	rapidxml::xml_node<>* node = mNode->last_node();
	if(node){
		return std::make_shared<XMLNode>(node);
	}
	else{
		return nullptr;
	}
}	

XMLNodePtr XMLNode::PrevSibling( const std::string& name )
{
	rapidxml::xml_node<>* node = mNode->previous_sibling(name.c_str());
	if(node){
		return std::make_shared<XMLNode>(node);
	}
	else{
		return nullptr;
	}
}

XMLNodePtr XMLNode::PrevSibling()
{
	rapidxml::xml_node<>* node = mNode->previous_sibling();
	if(node){
		return std::make_shared<XMLNode>(node);
	}
	else{
		return nullptr;
	}
}

XMLNodePtr XMLNode::NextSibling( const std::string& name )
{
	rapidxml::xml_node<>* node = mNode->next_sibling(name.c_str());
	if(node){
		return std::make_shared<XMLNode>(node);
	}
	else{
		return nullptr;
	}
}

XMLNodePtr XMLNode::NextSibling()
{
	rapidxml::xml_node<>* node = mNode->next_sibling();
	if(node){
		return std::make_shared<XMLNode>(node);
	}
	else{
		return nullptr;
	}
}

void XMLNode::InsertNode( const XMLNodePtr& where, const XMLNodePtr& child )
{
	mNode->insert_node(where->mNode, child->mNode);
}

void XMLNode::AppendNode( const XMLNodePtr& child )
{
	mNode->append_node(child->mNode);
}

void XMLNode::AppendAttribute( const XMLAttributePtr& attribute )
{
	mNode->append_attribute(attribute->mAttribute);
}

void XMLNode::InsertAttribute( const XMLAttributePtr& where, const XMLAttributePtr& attribute )
{
	mNode->insert_attribute(where->mAttribute, attribute->mAttribute);
}

void XMLNode::RemoveFirstNode()
{
	if(mNode->first_node())
	{
		mNode->remove_first_node();
	}
}

void XMLNode::RemoveLastNode()
{
	if(mNode->first_node())
	{
		mNode->remove_last_node();
	}
}

void XMLNode::RemoveNode( const XMLNodePtr& where )
{
	mNode->remove_node(where->mNode);
}

void XMLNode::RemoveAllNodes()
{
	mNode->remove_all_nodes();
}

void XMLNode::RemoveFirstAttribute()
{
	if(mNode->first_attribute())
	{
		mNode->remove_first_attribute();
	}

}

void XMLNode::RemoveLastAttribute()
{
	if(mNode->first_attribute())
	{
		mNode->remove_last_attribute();
	}
}

void XMLNode::RemoveAttribute( const XMLAttributePtr& where )
{
	mNode->remove_attribute(where->mAttribute);
}

void XMLNode::RemoveAllAttributes()
{
	mNode->remove_all_attributes();
}

uint32_t XMLNode::ValueUInt() const
{
	return LexicalCast<uint32_t>(ValueString());
}

int32_t XMLNode::ValueInt() const
{
	return LexicalCast<int32_t>(ValueString());
}

float XMLNode::ValueFloat() const
{
	return LexicalCast<float>(ValueString());
}

std::string XMLNode::ValueString() const
{
	return std::string(mNode->value(), mNode->value_size());
}

XMLNodePtr XMLNode::GetParent()
{
	rapidxml::xml_node<>* node = mNode->parent();
	if(node){
		return std::make_shared<XMLNode>(node);
	}
	else{
		return nullptr;
	}
}

	
XMLAttribute::XMLAttribute( rapidxml::xml_attribute<>* attr )
	: mAttribute(attr)
{
}

std::string XMLAttribute::Name() const
{
	assert(mAttribute);
	return std::string(mAttribute->name(), mAttribute->name_size());
}

XMLAttributePtr XMLAttribute::PrevAttribute( const std::string& name ) const
{
	rapidxml::xml_attribute<>* attr = mAttribute->previous_attribute(name.c_str());
	if(attr){
		return std::make_shared<XMLAttribute>(attr);
	}
	else{
		return nullptr;
	}
}

XMLAttributePtr XMLAttribute::PrevAttribute() const
{
	rapidxml::xml_attribute<>* attr = mAttribute->previous_attribute();
	if(attr){
		return std::make_shared<XMLAttribute>(attr);
	}
	else{
		return nullptr;
	}
}

XMLAttributePtr XMLAttribute::NextAttribute( const std::string& name ) const
{
	rapidxml::xml_attribute<>* attr = mAttribute->next_attribute(name.c_str());
	if(attr){
		return std::make_shared<XMLAttribute>(attr);
	}
	else{
		return nullptr;
	}
}

XMLAttributePtr XMLAttribute::NextAttribute() const
{
	rapidxml::xml_attribute<>* attr = mAttribute->next_attribute();
	if(attr){
		return std::make_shared<XMLAttribute>(attr);
	}
	else{
		return nullptr;
	}
}

uint32_t XMLAttribute::ValueUInt() const
{
	return LexicalCast<uint32_t>(ValueString());	
}

int32_t  XMLAttribute::ValueInt()    const
{
	return LexicalCast<int32_t>(ValueString());
}

float  XMLAttribute::ValueFloat()  const
{
	return LexicalCast<float>(ValueString());
}

std::string XMLAttribute::ValueString() const
{
	return std::string(mAttribute->value(), mAttribute->value_size());
}


} // Namespace RcEngine