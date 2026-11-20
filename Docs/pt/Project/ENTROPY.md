# Versionamento por Entropy

A Cosmeron utiliza **Entropy** como medida das mudanças acumuladas na biblioteca.

> **Entropy mede mudanças acumuladas, não o tamanho do código.**

Um estado publicado é escrito como:

```text
Entropy: <H absoluto><faixa grega>
```

Por exemplo: `Entropy: 18866β`.

## Escopo canônico

O escopo do código-fonte da Cosmeron é:

```text
Codespace/Cosmeron/**
```

Testes, documentação, traduções, configuração de CI, badges gerados e metadados do repositório não afetam a Entropy.

## Origem

A primeira versão canônica da Cosmeron estabelece a tag Git imutável `entropy-origin`. Cada linha rastreada em `Codespace/Cosmeron/**` nesse ponto contribui exatamente **1 H**:

```text
H₀ = linhas em Codespace/Cosmeron/** em entropy-origin
```

O histórico anterior à origem da Cosmeron não é reconstruído no novo valor público de Entropy. O desenvolvimento histórico permanece disponível no repositório predecessor e no histórico Git; a série da Cosmeron começa em seu próprio estado canônico de código-fonte.

## Alterações após a origem

Para cada commit no histórico canônico de primeiro pai:

```text
ΔH = adições + exclusões
Hₙ = H₀ + ΣΔH
```

Assim:

- linha adicionada: **+1 H**;
- linha removida: **+1 H**;
- linha reescrita: **+2 H**.

Uma linha reescrita contribui duas vezes porque o Git a representa como um estado removido e outro introduzido. Uma refatoração pode reduzir o tamanho do código-fonte e ainda aumentar a Entropy.

A Entropy é acumulada commit a commit. Alterar uma linha e depois restaurar o texto original ainda aumenta a Entropy, pois a biblioteca passou por estados adicionais.

O cálculo canônico segue o histórico de **primeiro pai**, portanto pull requests integrados por squash contam a transição de estado que efetivamente entrou na linha principal.

## Faixas gregas

Cada **10.000 H** seleciona uma faixa grega:

```text
0–9.999 H          α
10.000–19.999 H    β
20.000–29.999 H    γ
...
230.000–239.999 H  ω
```

Após `ω`, os símbolos continuam como colunas de planilha, usando um alfabeto bijetivo de base 24: `ω → αα → αβ → αγ → ...`. O número absoluto nunca é reiniciado.

## Reprodutibilidade

A implementação pertence a `.github/entropy/` e deve derivar a base de `entropy-origin`, somar adições e exclusões de `git diff --numstat` no histórico de primeiro pai e manter o estado gerado fora do escopo canônico do código-fonte.

`.gitattributes` fixa a árvore de código-fonte em LF para impedir que conversões CRLF/LF inflem artificialmente a Entropy.

Os testes automatizados do algoritmo devem cobrir snapshot inicial, adições, exclusões, reescritas, arquivos fora do escopo, renomeações, reversões, transições de faixa grega e formato público de exibição.

## Identidade das versões

A Cosmeron não utiliza versionamento semântico para versões canônicas. Uma versão é identificada pelo estado de Entropy em que é publicada.

Consulte [RELEASES.md](RELEASES.md) para o registro de versões.
