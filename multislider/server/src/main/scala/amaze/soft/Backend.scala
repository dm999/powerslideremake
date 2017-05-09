package amaze.soft

import java.net.InetSocketAddress

import akka.actor.{Props, Actor, ActorRef}
import akka.io.{IO, Udp}
import akka.util.ByteString
import amaze.soft.BackendMessage.{Envelop, JsonMessage}
import amaze.soft.SessionActor.SignedEnvelop
import net.liftweb.json
import net.liftweb.json.{DefaultFormats, ShortTypeHints}
import org.slf4j.LoggerFactory

import scala.collection.mutable


/**
 * Created by Alexey on 22.05.2016.
 * The MIT License (MIT)
 * Copyright (c) 2016 Alexey Gruzdev
 */

object Backend {
  val MAX_SESSIONS_NUMBER = 256

  /**
   * Creates new session with specified name
   * Sender will get session id if success (id >= 0) or -1 in the case of fail
   */
  case class CreateSession(sessionName: String, players: List[String])

  /**
   * Closes session.
   * No response
   */
  case class CloseSession(sessionId: Int)
}

class Backend(m_ip: String, m_port: Int) extends Actor {
  import Backend._
  import akka.io.Udp._

  private val m_logger = LoggerFactory.getLogger(Backend.getClass.getName)
  m_logger.info("Backend is created!")

  IO(Udp)(Depot.actorsSystem) ! Bind(self, new InetSocketAddress(m_ip, m_port))
  m_logger.info("Created UDP socket for " + m_ip + ":" + m_port)

  private val m_sessions     = new Array[SessionInfo](MAX_SESSIONS_NUMBER)
  private val m_freeSessions = mutable.Stack.range[Int](0, MAX_SESSIONS_NUMBER)
  private val m_sessionsLock = new Object()

  private implicit val formats = DefaultFormats.withHints(ShortTypeHints(List(classOf[Envelop])))

  def receive = {
    //------------------------------------------------------------------
    // Udp
    case Udp.Bound(local) => context become ready(sender())
    case _ => m_logger.info("[receive]: Got unknown message")
  }

  def ready(socket: ActorRef): Receive = {
    case Udp.Received(data, remote) =>
      //m_logger.info("Got a datagram")
      //m_logger.info("Datagram = " + data.decodeString(Constants.MESSAGE_ENCODING))
      try {
        val msg = json.parse(data.decodeString(Constants.MESSAGE_ENCODING)).extract[JsonMessage]
        //m_logger.info(msg.toString)
        msg match {
          //----------------------------------------------------------
          case Envelop(sessionId, packedData) =>
            //m_logger.info("Backend[ready]: Forward envelop for session " + sessionId)
            if(m_sessions(sessionId) != null) {
              m_sessions(sessionId).handler ! SignedEnvelop(socket, remote, packedData)
            }
          //----------------------------------------------------------
          case _ =>
            m_logger.error("Backend[ready]: Unknown message!")
        }
      } catch {
        case err: Exception =>
          m_logger.error(err.getMessage, err.getCause)
          err.printStackTrace()
          socket ! Udp.Send(ByteString(Constants.RESPONSE_SUCK), remote)
      }
    //----------------------------------------------------------
    // From lobby actors
    case CreateSession(sessionName, players) =>
      m_logger.info("Got a CreateSession message")
      val id = m_sessionsLock synchronized { if(m_freeSessions.nonEmpty) m_freeSessions.pop() else -1 }
      if(id >= 0) {
        m_sessions.update(id, SessionInfo(id, sessionName, Depot.actorsSystem.actorOf(Props(classOf[SessionActor], id, sessionName, players))))
        m_logger.info("Created session \"" + sessionName + "\" with id = " + id)
      }
      sender() ! id

    case CloseSession(sessionId) =>
      m_logger.info("Got a CloseSession message")
      if(m_sessions(sessionId) != null) {
        m_logger.info("Closed session \"" + m_sessions(sessionId).name + "\" with id = " + sessionId)
        m_sessions.update(sessionId, null)
        m_sessionsLock synchronized { m_freeSessions.push(sessionId)}
      } else {
        m_logger.error("Failed to close session with id = " + sessionId)
      }

    //----------------------------------------------------------
    case Udp.Unbind  =>
      socket ! Udp.Unbind

    case Udp.Unbound =>
      context stop self

    case _ =>
      m_logger.info("[ready]: Got unknown message")
  }

}
