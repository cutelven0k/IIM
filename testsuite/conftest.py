import pathlib

import pytest

from testsuite.databases.pgsql import discover

SERVICE_SOURCE_DIR = pathlib.Path(__file__).parent.parent

pytest_plugins = ['pytest_userver.plugins.postgresql']

@pytest.fixture(scope='session')
def pgsql_local(service_source_dir, pgsql_local_create):
    databases = discover.find_schemas(
        'V001__create_db',
        [service_source_dir.joinpath('schemas/postgresql/auth/migrations')],
    )
    return pgsql_local_create(list(databases.values()))