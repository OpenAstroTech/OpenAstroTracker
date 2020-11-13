package de.andrestefanov.openastrotracker.android

import de.andrestefanov.openastrotracker.android.repository.TelescopeRepository
import de.andrestefanov.openastrotracker.android.viewmodel.TelescopeListViewModel
import org.koin.androidx.viewmodel.dsl.viewModel
import org.koin.dsl.module

val module = module {

    // repositories
    single { TelescopeRepository() }

    // viewModels
    viewModel { TelescopeListViewModel() }
}