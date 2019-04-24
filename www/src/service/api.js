

//fetchMock.mock('http://example.com', 200);

export const getFoo = async () => {
    const res = await fetch('http://google.com');
    assert(res.ok);
    return res.json;
}