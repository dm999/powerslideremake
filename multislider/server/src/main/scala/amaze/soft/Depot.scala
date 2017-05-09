package amaze.soft

import java.util

import akka.actor.{ActorRef, ActorSystem}
import com.typesafe.config.ConfigFactory
import org.slf4j.LoggerFactory

/**
 * Created by Alexey on 20.05.2016.
 * The MIT License (MIT)
 * Copyright (c) 2016 Alexey Gruzdev
 */
object Depot {

  case class LobbyStats(handler: ActorRef, info: RoomInfo)

  object Status extends Enumeration {
    type Status = Value
    val SUCC, SUCK, ROOM_EXISTS = Value
  }
  import Status._

  private val m_logger = LoggerFactory.getLogger(Depot.getClass.getName)
  private val m_lock: Object = new Object
  //private val lobbies: util.TreeMap[String, RoomStats] = new util.TreeMap()
  // lobbyName -> Info
  private val m_lobbies: util.TreeMap[String, LobbyStats] = new util.TreeMap()

  // Max number of lobbies can be created
  val LOBBIES_MAX_NUMBER = 256

  val actorsSystem = ActorSystem("MultiSliderActors", ConfigFactory.load("akka.conf"))
  m_logger.info("Actors system is created!")

  var frontend: ActorRef = null
  var backend:  ActorRef = null
  var dogapi:   ActorRef = null

  var ip_address = ""
  var port_frontend = 0
  var port_backend = 0


  def registerLobby(handler: ActorRef, info: RoomInfo) : Status = {
    m_logger.info("Register lobby \"" + info.name + "\" created by \"" + info.host + "\"")
    m_lock.synchronized {
      if (m_lobbies.containsKey(info.name)) {
        return ROOM_EXISTS
      }
      if(m_lobbies.size() >= LOBBIES_MAX_NUMBER){
        return SUCK
      }
      m_lobbies.put(info.name, LobbyStats(handler, info))
    }
    SUCC
  }

  def unregisterLobby(roomName: String) = {
    m_logger.info("Remove lobby \"" + roomName + "\"")
    m_lock.synchronized {
      m_lobbies.remove(roomName)
    }
    m_logger.info("All lobbies = " + m_lobbies)
  }

  def updateRoomInfo(roomName: String, info: RoomInfo) : Boolean = {
    m_logger.info("Update lobby \"" + roomName + "\"")
    m_lock.synchronized {
      val old = m_lobbies.get(roomName)
      if(old != null) {
        m_lobbies.put(roomName, LobbyStats(old.asInstanceOf[LobbyStats].handler, info))
      } else {
        false
      }
    }
    true
  }

  def getLobbies = m_lobbies


  def getAddressFront = {
    ip_address + ":" + port_frontend.toString
  }

  def getAddressBack = {
    ip_address + ":" + port_backend.toString
  }

  def getIpBack = {
    ip_address
  }

  def getIpFront = {
    ip_address
  }

  def getPortFront = {
    port_frontend
  }

  def getPortBack = {
    port_backend
  }

}
