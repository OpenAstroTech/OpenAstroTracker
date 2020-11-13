package de.andrestefanov.openastrotracker.meade.network

import io.ktor.util.network.*
import io.ktor.utils.io.core.*

interface UDPSocket : Closeable {

    suspend fun send(data: ByteArray)

    suspend fun receive(buffer: ByteArray): Int

    interface Factory {

        fun create(address: NetworkAddress): UDPSocket

    }

}

