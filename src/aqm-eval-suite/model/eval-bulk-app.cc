/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 NITK Surathkal
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors: Ankit Deepak <adadeepak8@gmail.com>
 *          Shravya K. S. <shravya.ks0@gmail.com>
 *          Mohit P. Tahiliani <tahiliani@nitk.edu.in>
 */

#include "ns3/log.h"
#include "ns3/core-module.h"
#include "ns3/address.h"
#include "ns3/node.h"
#include "ns3/nstime.h"
#include "ns3/socket.h"
#include "ns3/simulator.h"
#include "ns3/socket-factory.h"
#include "ns3/packet.h"
#include "ns3/uinteger.h"
#include "ns3/trace-source-accessor.h"
#include "ns3/tcp-socket-factory.h"
#include "eval-bulk-app.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("EvalBulkApp");

NS_OBJECT_ENSURE_REGISTERED (EvalBulkApp);

EvalBulkApp::EvalBulkApp ()
  : m_socket (0),
    m_peer (),
    m_connected (false),
    m_maxBytes (0),
    m_totBytes (0)
{
  m_flowStop = Time::Min ();
  m_flowStart = Time::Min ();
}

EvalBulkApp::~EvalBulkApp ()
{
  m_socket = 0;
}

void
EvalBulkApp::Setup (Ptr<Socket> socket, Address address, uint32_t sendSize, uint64_t maxByte)
{
  m_socket = socket;
  m_peer = address;
  m_maxBytes = maxByte;
  m_sendSize = sendSize;
}

void
EvalBulkApp::DoDispose (void)
{
  NS_LOG_FUNCTION (this);

  m_socket = 0;
  // chain up
  Application::DoDispose ();
}

// Called at time specified by Start
void EvalBulkApp::StartApplication ()
{
  NS_LOG_FUNCTION (this);

  m_socket->Bind ();
  m_socket->Connect (m_peer);
  m_socket->ShutdownRecv ();

  if (m_flowStart == Time::Min ())
    {
      m_flowStart = Simulator::Now (); 
    }

  m_socket->SetConnectCallback (
    MakeCallback (&EvalBulkApp::ConnectionSucceeded, this),
    MakeCallback (&EvalBulkApp::ConnectionFailed, this));

  m_socket->SetSendCallback (
        MakeCallback (&EvalBulkApp::DataSend, this));
  
  if (m_connected)
    {
      SendData ();
    }
}

// Called at time specified by Stop
void EvalBulkApp::StopApplication ()
{
  NS_LOG_FUNCTION (this);

  if (m_socket != 0)
    {
      if (m_flowStop == Time::Min ())
        {
          m_flowStop = Simulator::Now ();
        }
      m_socket->Close ();
      m_connected = false;
    }
  else
    {
      NS_LOG_WARN ("EvalBulkApp found null socket to close in StopApplication");
    }
}

Time EvalBulkApp::GetFlowCompletionTime ()
{
  return m_flowStop - m_flowStart;
}




void EvalBulkApp::SendData ()
{
 NS_LOG_FUNCTION (this);

  while (m_maxBytes == 0 || m_totBytes < m_maxBytes)
    { // Time to send more

      // uint64_t to allow the comparison later.
      // the result is in a uint32_t range anyway, because
      // m_sendSize is uint32_t.
      uint64_t toSend = m_sendSize;
      // Make sure we don't send too many
      if (m_maxBytes > 0)
        {
          toSend = std::min (toSend, m_maxBytes - m_totBytes);
        }

      NS_LOG_LOGIC ("sending packet at " << Simulator::Now ());
      Ptr<Packet> packet = Create<Packet> (toSend);
      int actual = m_socket->Send (packet);
      if (actual > 0)
        {
          m_totBytes += actual;
        }
      // We exit this loop when actual < toSend as the send side
      // buffer is full. The "DataSent" callback will pop when
      // some buffer space has freed up.
      if ((unsigned)actual != toSend)
        {
          break;
        }
    }
  // Check if time to close (all sent)
  if (m_totBytes == m_maxBytes && m_connected)
    {
      m_socket->Close ();
      m_connected = false;
    }
}

void EvalBulkApp::ConnectionSucceeded (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  NS_LOG_LOGIC ("EvalBulkApp Connection succeeded");
  m_connected = true;
  SendData ();
}


void EvalBulkApp::ConnectionFailed (Ptr<Socket> socket)
{
  NS_LOG_FUNCTION (this << socket);
  NS_LOG_LOGIC ("EvalBulkApp, Connection Failed");
}
void EvalBulkApp::DataSend (Ptr<Socket>, uint32_t)
{
  NS_LOG_FUNCTION (this);
  if (m_connected)
    { // Only send new data if the connection has completed
      SendData ();
    }
}
} //namespace ns3
