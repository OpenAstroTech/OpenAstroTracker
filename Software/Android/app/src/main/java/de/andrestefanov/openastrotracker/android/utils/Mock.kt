package de.andrestefanov.openastrotracker.android.utils

import de.andrestefanov.openastrotracker.meade.network.Connection

class MockTCPConnection(private val host: String = "127.0.0.1", private val port: Int = 4030) :
    Connection {

    override val address: String
        get() = "$host:$port"

    override val protocol: Connection.Protocol
        get() = Connection.Protocol.TCP

    override suspend fun command(command: String) = Unit

    override suspend fun commandWithResponse(command: String) = when (command) {
        ":CM#" -> "NONE#"
        else -> TODO("Mock command $command not yet implemented")
    }

}