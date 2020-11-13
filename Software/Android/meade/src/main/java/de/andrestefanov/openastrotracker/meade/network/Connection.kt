package de.andrestefanov.openastrotracker.meade.network

interface Connection {

    interface Factory {
        fun create(address: String): Connection
    }

    enum class Protocol(val factory: Factory) {
        TCP(TCPConnection.FACTORY)
    }

    val address: String

    val protocol: Protocol

    suspend fun command(command: String)

    suspend fun commandWithResponse(command: String): String

    companion object {
        fun from(type: String, address: String) = Protocol.valueOf(type).factory.create(address)
    }
}