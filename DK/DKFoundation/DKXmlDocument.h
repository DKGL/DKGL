//
//  File: DKXmlDocument.h
//  Author: Hongtae Kim (tiff2766@gmail.com)
//
//  Copyright (c) 2004-2016 Hongtae Kim. All rights reserved.
//

#pragma once
#include "../DKInclude.h"
#include "DKObject.h"
#include "DKString.h"
#include "DKData.h"
#include "DKXmlParser.h"

namespace DKFoundation
{
	/// XML DOM class, provides parse and generate DOM of XML, HTML.
	/// this class uses DKXmlParser internally. (see DKXmlParser.h)
	/// this class provides DOM includes DTD.
	class DKGL_API DKXmlDocument
	{
	public:
		enum Type
		{
			TypeXML,
			TypeHTML,
		};
		struct Namespace
		{
			DKString	prefix;
			DKString	URI;
		};
		struct Attribute
		{
			DKObject<Namespace>	ns;
			DKString			name;
			DKString			value;
		};
		class Node
		{
		public:
			virtual ~Node() {}
			enum NodeType
			{
				NodeTypeUnknown = 0,
				NodeTypeInstruction,
				NodeTypeDocTypeDecl,
				NodeTypeElementDecl,
				NodeTypeAttributeDecl,
				NodeTypeComment,
				NodeTypeElement,
				NodeTypeCData,  ///< CDATA section strings ignored by parser.
				NodeTypePCData, ///< Parsed Character Data. (some symbols will be translated.)
			};
			NodeType			Type() const;
			virtual DKString	Export() const = 0;
		protected:
			Node(NodeType t) : type(t) {}
		private:
			const NodeType	type;
		};
		struct CData : public Node
		{
			CData() : Node(NodeTypeCData) {}
			DKStringU8 value;		///< UTF-8 string
			DKString Export() const;
		};
		struct PCData : public Node
		{
			PCData() : Node(NodeTypePCData) {}
			DKString value;
			DKString Export() const;
		};
		struct Comment : public Node
		{
			Comment() : Node(NodeTypeComment) {}
			DKString value;
			DKString Export() const;
		};
		struct Element : public Node
		{
			Element() : Node(NodeTypeElement) {}
			DKObject<Namespace>				ns;
			DKString						name;
			DKArray<Attribute>				attributes;
			DKArray<Namespace> 				namespaces;
			DKArray<DKObject<Node>>		nodes;
			DKString Export() const;
			DKString Export(DKArray<Namespace>& writtenNS) const;
		};
		struct Instruction : public Node
		{
			Instruction() : Node(NodeTypeInstruction) {}
			DKString						target;
			DKString						data;
			DKString Export() const;
		};
		struct ElementDecl : public Node		///< DTD Element
		{
			ElementDecl() : Node(NodeTypeElementDecl) {}
			DKXmlParser::ElementDecl		decl;
			DKXmlParser::ElementContentDecl	contents;
			DKString Export() const;
		};
		struct AttributeDecl : public Node	///< DTD
		{
			AttributeDecl() : Node(NodeTypeAttributeDecl) {}
			DKXmlParser::AttributeDecl		decl;
			DKArray<DKString>				enumeratedValues;
			DKString Export() const;
		};
		struct DocTypeDecl : public Node
		{
			DocTypeDecl() : Node(NodeTypeDocTypeDecl) {}
			DKString					name;
			DKString					externalID;
			DKString					systemID;
			DKArray<DKObject<Node>>		nodes;
			DKString Export() const;
		};
		DKXmlDocument();
		DKXmlDocument(DocTypeDecl* dtd, Element* root);
		DKXmlDocument(Element* root);
		~DKXmlDocument();

		/// open and create object with URL or file.
		static DKObject<DKXmlDocument> Open(Type t, const DKString& fileOrURL, DKString* desc = NULL);

		/// When reading HTML from buffer, they should be encoded with UTF-8.
		/// becouse XML has encoding information, but HTML does not.
		static DKObject<DKXmlDocument> Open(Type t, const DKData* buffer, DKString* desc = NULL);
		static DKObject<DKXmlDocument> Open(Type t, DKStream* stream, DKString* desc = NULL);

		DKObject<DKData> Export(DKStringEncoding e) const;
		size_t Export(DKStringEncoding e, DKStream* output) const;

		////////////////////////////////////////////////////////////////////////////////
		Element*			RootElement();
		const Element*		RootElement() const;
		void				SetRootElement(Element* e);				
		// DTD subset
		void				SetDtd(DocTypeDecl* d);				
		DocTypeDecl*		Dtd();
		const DocTypeDecl*	Dtd() const;

	private:
		DKArray<DKObject<Node>> nodes; // all nodes of DOM.
		class DocumentBuilder;
	};

	typedef DKXmlDocument::Namespace		DKXmlNamespace;
	typedef DKXmlDocument::Attribute		DKXmlAttribute;
	typedef DKXmlDocument::Node				DKXmlNode;
	typedef DKXmlDocument::CData			DKXmlCData;
	typedef DKXmlDocument::PCData			DKXmlPCData;
	typedef DKXmlDocument::Comment			DKXmlComment;
	typedef DKXmlDocument::Element			DKXmlElement;
	typedef DKXmlDocument::Instruction		DKXmlInstruction;
	typedef DKXmlDocument::ElementDecl		DKXmlElementDecl;
	typedef DKXmlDocument::AttributeDecl	DKXmlAttributeDecl;
	typedef DKXmlDocument::DocTypeDecl		DKXmlDocTypeDecl;
}
