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

#ifndef EVAL_BULK_APPLICATION_H
#define EVAL_BULK_APPLICATION_H

#include "ns3/address.h"
#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/traced-callback.h"
//#include "ns3/data-rate.h"
#include "ns3/random-variable-stream.h"

namespace ns3 {

/**
 * \brief Generates traffic in the suite
 *
 * This class generates TCP and UDP traffic in the suite. It has been
 * designed to overcome the challenges faced while configuring attributes
 * of TCP sockets, such as the initial congestion window.
 */
 class EvalBulkApp : public Application
 {
 public:
  /**
   * \brief Constructor
   */
   EvalBulkApp ();

  /**
   * \brief Destructor
   */
   virtual ~EvalBulkApp ();

  /**
   * \brief Configures the application
   *
   * \param socket The socket to be used by the application
   * \param address Remote address
   * \param packetSize The size of payload
   * \param maxByte The maximum amount of data to be sent by the application
   * \param stream The stream used by RNG
   */
   void Setup (Ptr<Socket> socket, Address address, uint32_t sendSize, uint64_t maxByte);
 
  /**
   * \brief Get the flow completion time
   *
   * \return The flow completion time
   */
   Time GetFlowCompletionTime ();

  friend class EvaluationTopology;

protected:
  virtual void DoDispose (void);

private:
  /**
   * \brief Starts the application
   */
   virtual void StartApplication (void);

  /**
   * \brief Stops the application
   */
   virtual void StopApplication (void);


  void SendData ();

  Ptr<Socket>     m_socket;                   //!< Associated socket
  Address         m_peer;                     //!< Peer address
  bool            m_connected;                //!< True if connected
  uint32_t        m_sendSize;     //!< Size of data to send each time
  //Ptr<RandomVariableStream>  m_onTime;        //!< rng for On Time
 // Ptr<RandomVariableStream>  m_offTime;       //!< rng for Off Time
  //DataRate        m_cbrRate;                  //!< Rate that data is generated
  //DataRate        m_cbrRateFailSafe;          //!< Rate that data is generated (check copy)
 // uint32_t        m_pktSize;                  //!< Size of packets
  //uint32_t        m_residualBits;             //!< Number of generated, but not sent, bits
  //Time            m_lastStartTime;            //!< Time last packet sent
  uint64_t        m_maxBytes;                 //!< Limit total number of bytes sent
  uint64_t        m_totBytes;                 //!< Total bytes sent so far
 // EventId         m_startStopEvent;           //!< Event id for next start or stop event
 // EventId         m_sendEvent;                //!< Event id of pending "send packet" event

private:
  Time            m_flowStart;                //!< Flow start time
  Time            m_flowStop;                 //!< Flow stop time

  

  void ConnectionSucceeded (Ptr<Socket> socket);

  /**
   * \brief Handle a Connection Failed event
   *
   * \param socket The not connected socket
   */
  void ConnectionFailed (Ptr<Socket> socket);
  void DataSend (Ptr<Socket>, uint32_t); // for socket's SetSendCallback
};

}

#endif /* EVAL_APPLICATION_H */
