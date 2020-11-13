package de.andrestefanov.openastrotracker.android.viewmodel

import androidx.lifecycle.ViewModel
import androidx.lifecycle.asLiveData
import de.andrestefanov.openastrotracker.android.repository.TelescopeRepository
import org.koin.core.context.GlobalContext

class TelescopeListViewModel : ViewModel() {

    // TODO: this is a workaround for koin 2.2.0 not working with compose 1.0.0-alpha07
    private val koin = GlobalContext.get()
    private val telescopeRepository = koin.get<TelescopeRepository>()

    val scopes = telescopeRepository.discoverScopes().asLiveData(timeoutInMs = 0)

}