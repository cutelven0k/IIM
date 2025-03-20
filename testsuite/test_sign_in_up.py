async def test_test(service_client):
    response = await service_client.get('/v1/user')
    assert response.status == 401