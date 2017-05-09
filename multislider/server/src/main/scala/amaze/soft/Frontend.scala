package amaze.soft

import java.net.InetSocketAddress

import akka.actor._
import akka.io.{IO, Tcp, Udp}
import akka.util.ByteString
import amaze.soft.FrontendMessage.{GetRooms, Greeting, JsonMessage}
import net.liftweb.json
import net.liftweb.json.{DefaultFormats, ShortTypeHints}
import org.slf4j.LoggerFactory

import scala.collection.JavaConversions._

/**
 * Created by Alexey on 19.05.2016.
 * The MIT License (MIT)
 * Copyright (c) 2016 Alexey Gruzdev
 */

object Frontend extends App {
  case class ClientDisconnected()

  private val MAX_CLIENTS_NUMBER = 1024
  private var m_clientsCounter = 0
  private val m_counterLock: Object = new Object

  private val m_logger = LoggerFactory.getLogger(Frontend.getClass.getName)

  // Cmd arguments: <ip> <frontend port> <backend port>
  Depot.ip_address    = if(args.length > 0) args(0) else "localhost"
  Depot.port_frontend = if(args.length > 1) args(1).toInt else 8800
  Depot.port_backend  = if(args.length > 2) args(2).toInt else 8700

  Depot.frontend = Depot.actorsSystem.actorOf(Props[Frontend], Constants.FRONTEND_NAME)
  Depot.backend  = Depot.actorsSystem.actorOf(Props(classOf[Backend], Depot.ip_address, Depot.port_backend), Constants.BACKEND_NAME)

  Depot.dogapi = Depot.actorsSystem.actorOf(Props[DogApi], Constants.DOG_API_NAME)
  //val address = Address("akka.tcp", Depot.actorsSystem.name, Constants.DOG_API_NAME, 8600)
  //Depot.dogapi = Depot.actorsSystem.actorOf(Props[DogApi].withDeploy(Deploy(scope = RemoteScope(address))), Constants.DOG_API_NAME)
  //m_logger.info("API = " + Depot.dogapi.path)
}


class Frontend extends Actor {
  import Frontend._

  private implicit val formats = DefaultFormats.withHints(ShortTypeHints(List(classOf[GetRooms], classOf[Greeting])))

  m_logger.info("Frontend is created!")
  IO(Tcp)(Depot.actorsSystem) ! Tcp.Bind(self, new InetSocketAddress(Depot.ip_address, Depot.port_frontend))
  m_logger.info("Created TCP socket for " + Depot.ip_address + ":" + Depot.port_frontend)
  IO(Udp)(Depot.actorsSystem) ! Udp.Bind(self, new InetSocketAddress(Depot.ip_address, Depot.port_frontend))
  m_logger.info("Created UDP socket for " + Depot.ip_address + ":" + Depot.port_frontend)

  def receive = {
    case Tcp.Bound(localAddress) =>
      m_logger.info("Listening to " + localAddress)

    case Tcp.CommandFailed(_: Tcp.Bind) =>
      m_logger.info("Fail"); context stop self

    case Tcp.Connected(remote, local) =>
      m_logger.info("remote = " + remote)
      m_logger.info("local  = " + local)

      var status = false
      m_counterLock.synchronized {
        if (m_clientsCounter < MAX_CLIENTS_NUMBER) {
          m_clientsCounter += 1
          status = true
        }
      }
      val client = sender()
      if(status) {
        m_logger.info("Clients number = " + m_clientsCounter)
        val handler = context.actorOf(Props(classOf[Client]))
        client ! Tcp.Register(handler)
        client ! Tcp.Write(ByteString(json.Serialization.write(
          Greeting(Constants.DEFAULT_NAME, Constants.VERSION_MAJOR, Constants.VERSION_MINOR, Constants.VERSION_REVISION))))
      } else {
        client ! Tcp.Write(ByteString(Constants.RESPONSE_SUCK_IS_FULL))
        client ! Tcp.Close
      }


    case ClientDisconnected() =>
      m_counterLock.synchronized {
        m_clientsCounter = math.max(0, m_clientsCounter - 1)
      }
      m_logger.info("Clients number = " + m_clientsCounter)


    case Udp.Bound(local) =>
      m_logger.info("UDP socket is bound")

    case Udp.Received(data, remote) =>
      m_logger.info("Got a datagram")
      m_logger.info("Datagram = " + data.decodeString(Constants.MESSAGE_ENCODING))
      try {
        val msg = json.parse(data.decodeString(Constants.MESSAGE_ENCODING)).extract[JsonMessage]
        m_logger.info(msg.toString)
        msg match {
          case GetRooms() =>
            m_logger.info("Got a GetRooms message!")
            sender() ! Udp.Send(ByteString(json.Serialization.write(Depot.getLobbies.map({
              case (_, lobby) => lobby.info.noPlayers
            }).toList)), remote)

          case _ =>
            m_logger.error("Unknown message!")
        }
      } catch {
        case err: Exception =>
          m_logger.error(err.getMessage, err.getCause)
          err.printStackTrace()
          sender() ! Tcp.Write(ByteString(Constants.RESPONSE_SUCK))
      }

    case Udp.Unbind  =>
      sender() ! Udp.Unbind
  }

}
