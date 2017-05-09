package amaze.soft

import akka.actor.{ActorRef, Actor, Props}
import akka.io.Tcp
import akka.io.Tcp.{ErrorClosed, PeerClosed}
import akka.util.ByteString
import amaze.soft.FrontendMessage._
import net.liftweb.json
import net.liftweb.json.{DefaultFormats, ShortTypeHints}
import org.slf4j.LoggerFactory

/**
 * Created by Alexey on 12.07.2016.
 * The MIT License (MIT)
 * Copyright (c) 2016 Alexey Gruzdev
 */

object Client
{
  private val m_logger = LoggerFactory.getLogger(this.getClass.getName)
}

class Client extends Actor {
  import Client._

  private implicit val formats = DefaultFormats.withHints(ShortTypeHints(List(
    classOf[CreateRoom], classOf[CloseRoom], classOf[JoinRoom], classOf[Envelop])))

  private var m_handler: ActorRef = null

  override def receive = {

    case Tcp.Received(data) =>
      try {
        val msg = json.parse(data.decodeString(Constants.MESSAGE_ENCODING)).extract[JsonMessage]
        m_logger.info(msg.toString)
        msg match {
          case CreateRoom(playerName, roomName, description, playersLimit, playersReserved, userParam) =>
            m_logger.info("Got a CreateRoom message!")
            m_handler = Depot.actorsSystem.actorOf(Props(classOf[LobbyActor2]))
            m_handler forward LobbyActor2.Init(playerName, roomName, description, playersLimit, playersReserved, userParam)

          case join @ JoinRoom(playerName, roomName) =>
            m_logger.info("Got a JoinRoom message!")
            val lobby = Depot.getLobbies.get(roomName)
            if(lobby != null) {
              m_handler = lobby.handler
              lobby.handler forward LobbyActor2.Join(playerName)
            } else {
              sender() ! Tcp.Write(ByteString(Constants.RESPONSE_SUCK))
            }

          case Envelop(roomName, messageData) =>
            m_logger.info("Got a Envelop message!")
            val lobby = Depot.getLobbies.get(roomName)
            if(lobby != null) {
              lobby.handler forward messageData
            } else {
              m_logger.warn("Room " + roomName + " was not found! Message wont be delivered")
            }

          case CloseRoom() =>
            m_logger.info("Got a CloseRoom message!")
            if(m_handler != null){
              m_handler forward LobbyActor2.Close()
              m_handler = null
            }

          case _ =>
            m_logger.error("Unknown message!")
        }
      } catch {
        case err: Exception =>
          m_logger.error(err.getMessage, err.getCause)
          err.printStackTrace()
          sender() ! Tcp.Write(ByteString(Constants.RESPONSE_SUCK))
      }

    case PeerClosed =>
      shutdown()

    case ErrorClosed(cause) =>
      m_logger.warn("Connection is closed; Reason = " + cause)
      shutdown()

    case _ =>
      m_logger.error("Unknown message!")

  }

  def shutdown() = {
    if(m_handler != null){
      m_handler forward LobbyActor2.Disconnected()
    }
    Depot.frontend ! Frontend.ClientDisconnected()
    context stop self
  }

}
