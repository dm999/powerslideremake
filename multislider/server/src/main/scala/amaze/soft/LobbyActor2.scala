package amaze.soft

import akka.actor.{Actor, ActorRef}
import akka.io.Tcp
import akka.pattern.ask
import akka.util.{Timeout, ByteString}
import amaze.soft.Backend.CreateSession
import amaze.soft.FrontendMessage._
import net.liftweb.json
import net.liftweb.json.{DefaultFormats, ShortTypeHints}
import org.slf4j.LoggerFactory

import scala.collection.mutable.ListBuffer
import scala.concurrent.Await

/**
 * Created by Alexey on 11.07.2016.
 * The MIT License (MIT)
 * Copyright (c) 2016 Alexey Gruzdev
 */

object LobbyActor2 {
  case class PlayerInfo2(actor: ActorRef, name: String, ready: Boolean)
  case class Init(hostName: String, roomName: String, description: String, playersLimit: Int, playersReserved: Int, userParam: String)
  case class Join(playerName: String)
  case class Close()
  case class Disconnected()
  case class Shutdown()

  private val m_logger = LoggerFactory.getLogger(this.getClass.getName)
}


class LobbyActor2() extends Actor {
  import LobbyActor2._

  var m_name: String = null
  var m_description: String = null
  var m_players = new ListBuffer[PlayerInfo2]
  var m_playersLimit = 0
  var m_playersReserved = 0
  var m_userParam: String = null

  m_logger.info("Started actor " + self.toString())

  private implicit val formats = DefaultFormats.withHints(ShortTypeHints(List(
    classOf[CloseRoom], classOf[JoinRoom], classOf[LeaveRoom], classOf[EjectPlayer], classOf[StartSession], classOf[SessionStarted],
    classOf[Update], classOf[Ejected], classOf[Message], classOf[Reconfigure], classOf[ReconfigureSuck], classOf[ReconfigureSucc])))

  private def getHost = m_players.head

  private def makeRoomInfo() = new RoomInfo(m_name, getHost.name, m_description, m_playersLimit, m_playersReserved, m_players.length, m_players.map{_.name}.toList, m_userParam)

  private def ejectPlayer(player: PlayerInfo2, flags: Int) = {
    player.actor ! Tcp.Write(ByteString(json.Serialization.write(Ejected(flags))))
  }

  private def shutdown() = {
    m_logger.info("Stopped actor " + self.toString())
    context stop self
  }

  override def receive = {
    case Init(hostName, roomName, description, playersLimit, playersReserved, userParam) =>
      m_name = roomName
      m_description = description
      m_playersLimit = playersLimit
      m_playersReserved = playersReserved
      m_players.append(PlayerInfo2(sender(), hostName, ready = false))
      m_userParam = userParam
      val room = makeRoomInfo()
      Depot.registerLobby(self, room) match {
        case Depot.Status.SUCC =>
          sender() ! Tcp.Write(ByteString(json.Serialization.write(room)))
          context become running
        case Depot.Status.ROOM_EXISTS =>
          sender() ! Tcp.Write(ByteString(Constants.RESPONSE_ROOM_EXISTS))
          context become zombie
        case _ =>
          sender() ! Tcp.Write(ByteString(Constants.RESPONSE_SUCK))
          context become zombie
      }

    case _ =>
      m_logger.error("Unknown message")
  }

  def running : Receive = {
    case jsonRaw : String =>
      try {
        val msg = json.parse(jsonRaw).extract[JsonMessage]
        //m_logger.info(msg.toString)
        msg match {
          case updateMessage: Update =>
            self forward updateMessage

          case Message(sender, _, toSelf) =>
            m_logger.info("Got a Message message!")
            for(player <- m_players if toSelf || player.name != sender) {
              player.actor ! Tcp.Write(ByteString(jsonRaw))
            }

          case StartSession(data) =>
            m_logger.info("Got a StartSession message!")
            implicit val timeout = Timeout(Constants.FUTURE_TIMEOUT)
            val sessionId = Await.result(Depot.backend ? CreateSession(m_name, m_players.map{_.name}.toList), timeout.duration).asInstanceOf[Int]
            if (sessionId >= 0) {
              m_players.foreach { player =>
                player.actor ! Tcp.Write(ByteString(json.Serialization.write(SessionStarted(Depot.getIpBack, Depot.getPortBack, m_name, sessionId, data))))
              }
            } else {
              sender() ! Tcp.Write(ByteString(Constants.RESPONSE_SUCK))
            }

          case EjectPlayer(playerName) =>
            m_logger.info("Got a EjectPlayer message!")
            val host = getHost
            if((host.actor == sender()) && (host.name != playerName)){
              val player = m_players.find(_.name == playerName)
              if(player.isDefined){
                ejectPlayer(player.get, Constants.FLAG_EJECTED)
                m_players -= player.get
                self forward FrontendMessage.Update(makeRoomInfo(), "", playerName, toSelf = true, Constants.FLAG_LEFT)
              }
            }

          case Reconfigure(newLimit, newReserved) =>
            m_logger.info("Got a Reconfigure message!")
            if(getHost.actor == sender()){
              if(m_players.size > newLimit || m_players.size > newLimit - newReserved ) {
                sender() ! Tcp.Write(ByteString(json.Serialization.write(ReconfigureSuck())))
              } else {
                m_playersLimit = newLimit
                m_playersReserved = newReserved
                val room = makeRoomInfo()
                Depot.updateRoomInfo(m_name, room)
                m_players.foreach(player => player.actor ! Tcp.Write(ByteString(json.Serialization.write(ReconfigureSucc(makeRoomInfo())))))
              }
            }

          case LeaveRoom() =>
            m_logger.info("Got a LeaveRoom message!")
            self forward Disconnected()

          case unknown: Any =>
            m_logger.warn("Got unknown frontend message! Msg = " + unknown.toString)
        }
      } catch {
        case err: Exception =>
          m_logger.error(err.getMessage, err.getCause)
          err.printStackTrace()
          sender() ! Tcp.Write(ByteString(Constants.RESPONSE_SUCK))
      }

    case Update(_, data, senderName, toSelf, flags) =>
      m_logger.info("Got a Update message!")
      m_players.foreach{player =>
        if(toSelf || player.actor != sender()){
          player.actor ! Tcp.Write(ByteString(json.Serialization.write(Update(makeRoomInfo(), data, senderName, toSelf, flags))))
        }
      }

    case Join(playerName) =>
      m_logger.info("Got a Join message!")
      if(m_players.length < m_playersLimit - m_playersReserved){
        // Check that name is unique
        if(!m_players.exists(_.name == playerName)) {
          // Add the new player
          m_players += PlayerInfo2(sender(), playerName, ready = false)
          val room = makeRoomInfo()
          Depot.updateRoomInfo(m_name, room)
          sender() ! Tcp.Write(ByteString(json.Serialization.write(room)))
          // Notify all other players
          self forward FrontendMessage.Update(room, "", playerName, toSelf = false, Constants.FLAG_JOINED)
        } else {
          sender() ! Tcp.Write(ByteString(Constants.RESPONSE_NAME_EXISTS))
        }
      } else {
        sender() ! Tcp.Write(ByteString(Constants.RESPONSE_ROOM_IS_FULL))
      }

    case Disconnected() =>
      m_logger.info("Got a Disconnected message!")
      val player  = m_players.find(_.actor == sender())
      if(player.isDefined) {
        val newHost = player.get == getHost
        m_players -= player.get
        if (m_players.nonEmpty) {
          Depot.updateRoomInfo(m_name, makeRoomInfo())
          // Notify all other players
          self forward FrontendMessage.Update(makeRoomInfo(), "", player.get.name, toSelf = false,
            Constants.FLAG_LEFT | (if (newHost) Constants.FLAG_NEW_HOST else 0))
          if (newHost) {
            m_logger.info("New host! name = " + getHost.name)
          }
        } else {
          Depot.unregisterLobby(m_name)
          shutdown()
        }
      }

    case Close() =>
      m_logger.info("Got a Close message!")
      m_players.foreach{ player =>
        ejectPlayer(player, Constants.FLAG_EJECTED | Constants.FLAG_CLOSED_BY_HOST)
      }
      Depot.unregisterLobby(m_name)
      shutdown()

    case unknown: Any =>
      m_logger.warn("Got unknown message! Msg = " + unknown.toString)
  }

  def zombie : Receive = {
    case Disconnected() =>
      shutdown()

    case _ =>
      m_logger.warn("Ignoring message")
  }

}
