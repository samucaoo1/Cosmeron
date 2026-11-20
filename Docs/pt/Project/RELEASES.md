# Cosmeron — Registro de versões

A Cosmeron **não** utiliza versionamento semântico para suas versões canônicas.

As versões públicas são identificadas pela **Entropy** acumulada da biblioteca:

~~~text
Entropy: <H absoluto><faixa grega>
~~~

O valor numérico representa mudanças acumuladas em **H**; o sufixo grego identifica a faixa atual. Consulte [ENTROPY.md](ENTROPY.md) para conhecer o modelo completo.

## Estado atual

**Nenhuma versão canônica da Cosmeron foi publicada ainda.**

O repositório está sendo preparado a partir do estado mais recente do projeto anterior à migração. A primeira versão pública da Cosmeron estabelecerá a tag `entropy-origin` e seu valor inicial de entropia `H₀`.

O projeto predecessor utilizava o nome **Congro;Library**. Seus registros históricos de versões e desenvolvimento permanecem como contexto histórico; a série pública da Cosmeron começa com o primeiro estado de código-fonte publicado sob a identidade Cosmeron.

## Numeração de versões

O esquema canônico não utiliza números de versão `1.0.0`, `1.1.0` ou `2.0.0`.

Exemplos:

~~~text
18866β  estado de versão de exemplo
19124β  estado posterior
20037γ  estado posterior após uma mudança de faixa
~~~

O número absoluto de entropia nunca é reiniciado.

> Toda versão tem uma história. A Cosmeron mede quanta história foi acumulada.
