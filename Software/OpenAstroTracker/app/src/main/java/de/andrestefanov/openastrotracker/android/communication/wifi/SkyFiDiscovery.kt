package de.andrestefanov.openastrotracker.android.communication.wifi

import android.util.Log
import kotlinx.coroutines.ExperimentalCoroutinesApi
import kotlinx.coroutines.flow.combine
import kotlinx.coroutines.flow.flow
import java.net.*

class SkyFiDiscovery {

    data class SkyFiScope(val name: String, val address: InetAddress)

    companion object {
        private const val TAG = "SkyFiDiscovery"

        private const val TIMEOUT_MS = 1000
    }

    @Suppress("EXPERIMENTAL_API_USAGE")
    fun discover(port: Int) = combine(listAllAddresses().map { discover(it, port) }) {
        it.asList().flatten().toSet()
    }

    private fun listAllAddresses() = NetworkInterface.getNetworkInterfaces().toList()
        .filterNot { it.isLoopback }
        .filter { it.isUp }
        .map { it.interfaceAddresses }
        .flatten()
        .filterNot { it.broadcast == null }
        .filterNot { it.address == null }

    @Suppress("BlockingMethodInNonBlockingContext")
    @ExperimentalCoroutinesApi
    private fun discover(interfaceAddress: InterfaceAddress, port: Int) = flow {
        DatagramSocket(port).apply {
            broadcast = true
            soTimeout = TIMEOUT_MS
        }.use { socket ->
            Log.d(TAG, "listen for skyfi replies on ${socket.inetAddress}:${socket.port}")

            while (true) {
                // send broadcast packet
                Log.d(TAG, "sending discovery packet on ${interfaceAddress.broadcast}:$port")
                val buffer = "skyfi:?".toByteArray(Charsets.US_ASCII)
                val discoveryPacket = DatagramPacket(buffer, buffer.size, interfaceAddress.broadcast, port)
                socket.send(discoveryPacket)

                val result = mutableListOf<SkyFiScope>()

                while (true) {
                    val buf = ByteArray(256)
                    val packet = DatagramPacket(buf, buf.size)

                    try {
                        @Suppress("BlockingMethodInNonBlockingContext")
                        socket.receive(packet)
                    } catch (e: SocketTimeoutException) {
                        break
                    }

                    if (packet.address == interfaceAddress.address) continue

                    val message = String(packet.data, packet.offset, packet.length)

                    Log.d(TAG, "a device answered with: $message")

                    val payload = message.removePrefix("skyfi:")
                    val values = payload.split('@')
                    val deviceName = values[0]

                    result.add(SkyFiScope(deviceName, packet.address))
                }

                emit(result.toList())
            }
        }
    }
}