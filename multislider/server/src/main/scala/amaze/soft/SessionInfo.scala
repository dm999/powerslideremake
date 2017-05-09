package amaze.soft

import akka.actor.ActorRef

/**
 * Created by Alexey on 22.05.2016.
 * The MIT License (MIT)
 * Copyright (c) 2016 Alexey Gruzdev
 */
case class SessionInfo(id: Int, name: String, handler: ActorRef)
