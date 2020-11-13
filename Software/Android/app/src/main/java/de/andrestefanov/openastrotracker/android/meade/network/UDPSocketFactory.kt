package de.andrestefanov.openastrotracker.android.meade.network

import de.andrestefanov.openastrotracker.meade.network.UDPSocket
import io.ktor.util.network.*
import java.net.DatagramPacket
import java.net.DatagramSocket
import java.net.InetSocketAddress

private data class UDPSocketImpl(private val address: NetworkAddress) :
    DatagramSocket(address.port), UDPSocket {

    init {
        broadcast = true
    }

    override suspend fun send(data: ByteArray) {
        val address = InetSocketAddress(address.hostname, address.port)
        val packet = DatagramPacket(data, data.size, address)
        super.send(packet)
    }

    override suspend fun receive(buffer: ByteArray): Int {
        val packet = DatagramPacket(buffer, buffer.size)
        super.receive(packet)
        return packet.length
    }

}

class UDPSocketFactory : UDPSocket.Factory {

    override fun create(address: NetworkAddress): UDPSocket {
        return UDPSocketImpl(address)
    }

}