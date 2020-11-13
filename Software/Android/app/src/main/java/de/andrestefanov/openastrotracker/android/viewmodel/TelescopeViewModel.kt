package de.andrestefanov.openastrotracker.android.viewmodel

import androidx.lifecycle.LiveData
import androidx.lifecycle.ViewModel
import androidx.lifecycle.asLiveData
import de.andrestefanov.openastrotracker.meade.MeadeTelescope
import de.andrestefanov.openastrotracker.meade.response.DEC
import de.andrestefanov.openastrotracker.meade.response.RA
import kotlinx.coroutines.flow.combine

data class TelescopeState(
    val ra: RA = RA(0, 0, 0),
    val dec: DEC = DEC(0, 0, 0)
)

class TelescopeViewModel() : ViewModel() {

    fun data(telescope: MeadeTelescope): LiveData<TelescopeState> = with(telescope) {
        combine(
            getRA(),
            getDEC(),
            ::TelescopeState
        )
    }.asLiveData(timeoutInMs = 0)

}