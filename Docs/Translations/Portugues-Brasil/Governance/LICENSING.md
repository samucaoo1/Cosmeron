# Por que EUPL 1.2?

[Español (Latinoamérica)](../../Espanol-LATAM/Governance/LICENSING.md) · [English](../../../Governance/LICENSING.md)

Cosmeron é licenciada sob a **Licença Pública da União Europeia, Versão 1.2 somente (EUPL-1.2)**.

Este documento explica por que Cosmeron escolheu a EUPL. Ele é apenas informativo e não modifica, substitui ou complementa os termos presentes em [LICENSE](../../../../LICENSE).

## Por que EUPL?

### Uma licença para o projeto

A EUPL 1.2 se aplica à "Obra", isto é, à Obra Original ou às suas Obras Derivadas, e a licença trata expressamente essa Obra como uma obra ou software, em vez de limitar seu alcance somente a software executável.[^1]

Isso a torna adequada para o código-fonte da Cosmeron e para outros materiais protegidos por direitos autorais distribuídos como parte do projeto, como documentação e material técnico. A orientação oficial da EUPL também descreve a licença como abrangendo software e dados auxiliares.[^2]

### Reciprocidade sem isolar a biblioteca

Cosmeron foi projetada para ser útil como biblioteca dentro de projetos maiores, inclusive projetos distribuídos sob modelos de licenciamento diferentes.

A EUPL foi concebida com interoperabilidade em mente. A orientação oficial da EUPL explica que interfaces, APIs e estruturas de dados podem ser reutilizadas para implementar ligação estática ou dinâmica com componentes independentes sem alterar automaticamente a licença desses componentes independentes.[^2]

Essa orientação se apoia, em especial, na **Diretiva 2009/24/CE relativa à proteção jurídica dos programas de computador**. O considerando 10 descreve interoperabilidade como a capacidade de componentes de software e hardware trocarem informações e utilizarem mutuamente essas informações, enquanto o considerando 15 e o artigo 6 estabelecem uma exceção limitada de interoperabilidade quando a reprodução ou tradução do código é indispensável para obter as informações necessárias à interoperabilidade de um programa criado de forma independente com outros programas.[^3]

A orientação de compatibilidade da EUPL utiliza essas disposições ao tratar de linking. Ela também observa que não há jurisprudência específica do Tribunal de Justiça que resolva definitivamente essa questão, portanto a Cosmeron apresenta isso como interpretação e orientação oficial da EUPL, e não como uma decisão judicial.[^4]

Ao mesmo tempo, quando modificações ou obras derivadas cobertas pela EUPL são distribuídas ou comunicadas ao público, o artigo 5 preserva obrigações recíprocas sobre a Obra coberta e exige que o código-fonte seja fornecido ou mantido livremente acessível.[^1]

Na prática, o objetivo do projeto é:

    Use Cosmeron livremente.
    Integre Cosmeron livremente.
    Melhore Cosmeron livremente.
    Compartilhe melhorias distribuídas da Cosmeron.

### Protegendo melhorias compartilhadas

A natureza recíproca da EUPL ajuda a manter melhorias e obras derivadas distribuídas da Obra coberta disponíveis sob termos abertos. O artigo 5 exige que a Distribuição ou Comunicação da Obra coberta ou de Obras Derivadas permaneça sob a EUPL, salvo quando o mecanismo de compatibilidade for aplicável.[^1]

Para Cosmeron, isso favorece uma base técnica compartilhada sem impedir que a biblioteca participe de sistemas maiores e com licenças diferentes.

### Compatibilidade

A EUPL 1.2 inclui no artigo 5 um mecanismo explícito de compatibilidade com licenças livres e de código aberto recíprocas listadas em seu Apêndice.[^1]

O Apêndice inclui GPL v2/v3, AGPL v3, LGPL v2.1/v3, MPL 2.0, EPL 1.0, CeCILL 2.0/2.1, OSL 2.1/3.0, CC BY-SA 3.0 para obras que não sejam software, EUPL 1.1/1.2 e LiLiQ-R/R+.[^1]

A Obra original coberta pela EUPL continua sob a EUPL. O mecanismo de compatibilidade existe para permitir a distribuição de obras combinadas maiores quando as obrigações das licenças aplicáveis poderiam entrar em conflito.

### Uso em rede

A definição de "Distribuição ou Comunicação" da EUPL inclui expressamente disponibilizar a Obra online ou colocar suas funcionalidades essenciais à disposição de outra pessoa.[^1] Por isso, a orientação oficial da EUPL descreve a EUPL 1.2 como abrangendo SaaS e distribuição por rede.[^2]

Assim, sua reciprocidade não se limita apenas à distribuição tradicional de cópias de código-fonte ou executáveis.

### Multilíngue por concepção

A EUPL 1.2 possui **23 versões linguísticas oficiais com o mesmo valor jurídico**. O artigo 13 estabelece expressamente que as versões linguísticas aprovadas pela Comissão Europeia possuem valor idêntico e que as partes podem utilizar a versão de sua escolha.[^1][^5]

Isso é especialmente valioso para Cosmeron: a acessibilidade deve se aplicar não apenas ao código-fonte e à documentação, mas também à licença sob a qual o projeto é compartilhado.

### Cadeia de autoria

O artigo 6 contém uma garantia de cadeia de autoria: o Licenciante original garante que os direitos autorais relevantes lhe pertencem ou foram licenciados a ele e que possui autoridade para conceder a licença; cada Colaborador presta a mesma garantia quanto às suas modificações.[^1]

O Interoperable Europe descreve essa característica como um **Developer Certificate of Origin (DCO)**.[^2] Isso não deve ser confundido com um fluxo de contribuição que exija uma linha `Signed-off-by:` em cada commit; a Cosmeron poderá adotar esse procedimento separadamente, se desejar.

### Garantia e responsabilidade

A EUPL não depende de uma exclusão absoluta de responsabilidade. Os artigos 7 e 8 estabelecem a ausência de garantia na modalidade "as is" e uma exclusão de responsabilidade com exceções expressas para dolo, danos diretamente causados a pessoas naturais e regras legais de responsabilidade por produtos, quando aplicáveis.[^1]

O FAQ oficial da EUPL explica a lógica dessa redação: sob o direito europeu e dos Estados-Membros, uma exclusão total de responsabilidade pode ser inválida, razão pela qual a EUPL utiliza uma exclusão mais qualificada, destinada a funcionar dentro das normas imperativas aplicáveis.[^6]

### Fundamento jurídico europeu

A EUPL 1.2 foi adotada pela **Decisão de Execução (UE) 2017/863 da Comissão, de 18 de maio de 2017**. A decisão registra que a versão 1.2 foi adotada para facilitar ainda mais o compartilhamento e a reutilização, ampliar o escopo, esclarecer regras de lei aplicável e jurisdição e atualizar o apêndice de licenças compatíveis.[^7]

A própria licença especifica regras de jurisdição e lei aplicável nos artigos 14 e 15. Para licenciantes situados fora da União Europeia, o artigo 15 prevê a aplicação da lei belga, salvo acordo específico entre as partes.[^1]

Ainda assim, ela pode ser utilizada por projetos e desenvolvedores fora da União Europeia.

## Nossa intenção

Cosmeron busca equilibrar três princípios:

    liberdade para usar
        +
    liberdade para integrar
        +
    reciprocidade nas melhorias
        =
    desenvolvimento aberto sustentável

A EUPL 1.2 se encaixa bem nesse equilíbrio para uma biblioteca modular como a Cosmeron.

## Referências jurídicas e oficiais

[^1]: [Licença Pública da União Europeia v1.2 — texto oficial](https://interoperable-europe.ec.europa.eu/licence/european-union-public-licence-version-12-eupl), especialmente os artigos 1, 5–8 e 13–15 e o Apêndice.
[^2]: [Introdução à EUPL](https://interoperable-europe.ec.europa.eu/collection/eupl/introduction-eupl-licence), Interoperable Europe Portal.
[^3]: [Diretiva 2009/24/CE relativa à proteção jurídica dos programas de computador](https://eur-lex.europa.eu/eli/dir/2009/24/oj), especialmente os considerandos 10 e 15 e o artigo 6.
[^4]: [Matriz de licenças open source compatíveis com a EUPL](https://interoperable-europe.ec.europa.eu/collection/eupl/matrix-eupl-compatible-open-source-licences), Interoperable Europe Portal.
[^5]: [Textos oficiais da EUPL 1.2 em 23 línguas da UE](https://interoperable-europe.ec.europa.eu/collection/eupl/eupl-text-eupl-12).
[^6]: [FAQ da EUPL — exclusão de responsabilidade](https://interoperable-europe.ec.europa.eu/collection/eupl/faqs), Interoperable Europe Portal.
[^7]: [Decisão de Execução (UE) 2017/863 da Comissão](https://eur-lex.europa.eu/eli/dec_impl/2017/863/oj), Jornal Oficial da União Europeia.

Para os termos jurídicos, consulte sempre o texto oficial da EUPL 1.2 e o arquivo [LICENSE](../../../../LICENSE) deste repositório.
