#ifndef CUSTOMGRAPH_H
#define CUSTOMGRAPH_H

/*
 Copyright (c) 2008-2018, Benoit AUTHEMAN All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the author or Destrat.io nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL AUTHOR BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

//-----------------------------------------------------------------------------
// This file is a part of the QuickQanava software library.
//
// \file	cpp_sample.h
// \author	benoit@destrat.io
// \date	2018 05 24
//-----------------------------------------------------------------------------

// Qt headers
#include <QGuiApplication>
#include <QtQml>
#include <QQuickStyle>

// QuickQanava headers
#include <QuickQanava.h>
#include <QMap>

#include <QDebug>

#include "customnode.h"

class CustomGraph : public qan::Graph {
  Q_OBJECT
public:
  explicit CustomGraph(QQuickItem *parent = nullptr) : qan::Graph{parent} { /* Nil */ }
  virtual ~CustomGraph() override  { /* Nil */ }
private:
  CustomGraph(const CustomGraph &) = delete;
  QMap<qan::Node, qan::StyleManager> style_managers_map;
public:
  Q_INVOKABLE qan::Group*   insertCustomGroup();
  Q_INVOKABLE qan::Node*    insertCustomNode();
  Q_INVOKABLE qan::Edge*    insertCustomEdge(qan::Node* source, qan::Node* destination);
};

QML_DECLARE_TYPE(CustomGraph)

#endif // CUSTOMGRAPH_H
