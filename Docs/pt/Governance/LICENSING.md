# Por que EUPL 1.2?

[Português (Brasil)](../Translations/Portugues-Brasil/Governance/LICENSING.md) · [Español (Latinoamérica)](../Translations/Espanol-LATAM/Governance/LICENSING.md)

O Cosmeron é licenciado exclusivamente sob a **European Union Public Licence, versão 1.2 (EUPL-1.2)**.

Este documento explica por que o Cosmeron escolheu a EUPL. Seu conteúdo é apenas informativo e não modifica, substitui nem complementa os termos da [LICENSE](../../LICENSE).

## Por que a EUPL?

### Uma licença para o projeto

A EUPL 1.2 aplica-se à "Obra" (the Work), isto é, à Obra Original ou às suas Obras Derivadas, e a licença trata expressamente essa Obra como uma obra ou software, em vez de limitar seu escopo apenas ao software executável.[^1]

Isso a torna adequada ao código-fonte do Cosmeron e a outros materiais protegidos por direitos autorais distribuídos como parte do projeto, como documentação e material técnico. A orientação oficial da EUPL também descreve a licença como abrangendo software e dados auxiliares.[^2]

### Reciprocidade sem isolar a biblioteca

O Cosmeron foi concebido para ser útil como biblioteca dentro de projetos maiores, inclusive projetos distribuídos sob modelos de licenciamento diferentes.

A EUPL foi projetada tendo a interoperabilidade em vista. A orientação oficial da EUPL explica que interfaces, APIs e estruturas de dados podem ser reutilizadas para implementar linkedição estática ou dinâmica com componentes independentes sem alterar automaticamente a licença desses componentes independentes.[^2]

Essa orientação baseia-se, em particular, na **Diretiva 2009/24/CE relativa à proteção jurídica dos programas de computador**. O considerando 10 descreve interoperabilidade como a capacidade de componentes de software e hardware trocarem e utilizarem mutuamente informações; o considerando 15 e o artigo 6 estabelecem uma exceção limitada de interoperabilidade quando a reprodução ou tradução do código é indispensável para obter as informações necessárias à interoperabilidade de um programa criado independentemente com outros programas.[^3]

A orientação de compatibilidade da EUPL parte dessas disposições ao discutir a linkedição. Também observa que não há jurisprudência específica do Tribunal de Justiça que resolva definitivamente a questão da linkedição; portanto, o Cosmeron trata isso como interpretação e orientação oficial da EUPL, não como decisão judicial.[^4]

Ao mesmo tempo, quando modificações cobertas pela EUPL ou obras derivadas são distribuídas ou comunicadas ao público, o artigo 5 preserva as obrigações recíprocas relativas à Obra abrangida e exige que o código-fonte seja fornecido ou disponibilizado gratuitamente.[^1]

Na prática, o objetivo do projeto é:

    Usar o Cosmeron livremente.
    Integrar o Cosmeron livremente.
    Melhorar o Cosmeron livremente.
    Compartilhar melhorias distribuídas ao Cosmeron.

### Proteção das melhorias compartilhadas

A natureza recíproca da EUPL ajuda a manter disponíveis sob termos abertos as melhorias distribuídas e as obras derivadas da Obra abrangida. O artigo 5 exige que a distribuição ou comunicação da Obra abrangida ou de Obras Derivadas permaneça sob a EUPL, salvo quando o mecanismo de compatibilidade se aplicar.[^1]

Para o Cosmeron, isso sustenta uma base técnica compartilhada e, ao mesmo tempo, permite que a biblioteca participe de sistemas maiores com licenças diferentes.

### Compatibilidade

A EUPL 1.2 inclui um mecanismo explícito de compatibilidade no artigo 5 para licenças recíprocas de software livre e de código aberto listadas em seu Apêndice.[^1]

O Apêndice inclui GPL v2/v3, AGPL v3, LGPL v2.1/v3, MPL 2.0, EPL 1.0, CeCILL 2.0/2.1, OSL 2.1/3.0, CC BY-SA 3.0 para obras que não sejam software, EUPL 1.1/1.2 e LiLiQ-R/R+.[^1]

A Obra original coberta pela EUPL permanece sob a EUPL. O mecanismo de compatibilidade existe para permitir a distribuição de obras combinadas maiores quando as obrigações das licenças aplicáveis entrariam em conflito.

### Uso em rede

A definição de "Distribuição ou Comunicação" da EUPL inclui expressamente disponibilizar a Obra online ou fornecer a outra pessoa acesso às suas funcionalidades essenciais.[^1] Por isso, a orientação oficial da EUPL descreve a EUPL 1.2 como abrangendo SaaS e distribuição em rede.[^2]

Isso significa que sua reciprocidade não se limita à distribuição tradicional de cópias do código-fonte ou de executáveis.

### Multilíngue por concepção

A EUPL 1.2 possui **23 versões linguísticas oficiais com igual valor jurídico**. O artigo 13 determina expressamente que as versões linguísticas aprovadas pela Comissão Europeia têm valor idêntico e que as partes podem utilizar a versão de sua escolha.[^1][^5]

Isso é importante para o Cosmeron porque a acessibilidade deve abranger não apenas o código-fonte e a documentação, mas também a licença sob a qual o projeto é compartilhado.

### Cadeia de autoria

O artigo 6 contém uma garantia relativa à cadeia de autoria: o Licenciante original garante que os direitos autorais pertinentes lhe pertencem ou foram licenciados a ele e que possui autoridade para conceder a licença; cada Contribuidor oferece a mesma garantia para suas modificações.[^1]

O Interoperable Europe descreve esse recurso como um **Developer Certificate of Origin (DCO)**.[^2] Isso não deve ser confundido com um workflow de repositório que exija uma linha `Signed-off-by:` em cada commit; o Cosmeron pode adotar separadamente esse fluxo de contribuição, se desejar.

### Garantia e responsabilidade

A EUPL não depende de uma isenção absoluta de responsabilidade. Os artigos 7 e 8 estabelecem a exclusão de garantia "no estado em que se encontra" ("as is") e uma exclusão de responsabilidade com exceções expressas para dolo, danos causados diretamente a pessoas físicas e regras legais de responsabilidade por produtos, quando aplicáveis.[^1]

O FAQ oficial da EUPL explica a política por trás dessa redação: segundo o direito europeu e dos Estados-Membros, uma exclusão total de responsabilidade pode ser inválida; por isso, a EUPL utiliza uma exclusão mais qualificada, destinada a operar dentro do direito imperativo aplicável.[^6]

### Fundamento jurídico europeu

A EUPL 1.2 foi adotada pela **Decisão de Execução (UE) 2017/863 da Comissão, de 18 de maio de 2017**. A Decisão afirma que a versão 1.2 foi adotada para facilitar ainda mais o compartilhamento e a reutilização, ampliar o escopo, esclarecer as disposições sobre lei aplicável e jurisdição e atualizar o apêndice de licenças compatíveis.[^7]

A própria licença especifica regras de jurisdição e lei aplicável nos artigos 14 e 15. Para licenciantes de fora da União Europeia, o artigo 15 prevê a aplicação da lei belga, salvo acordo específico em contrário entre as partes.[^1]

Ainda assim, ela pode ser utilizada por projetos e desenvolvedores de fora da União Europeia.

## Nossa intenção

O Cosmeron busca equilibrar três princípios:

    liberdade de uso
        +
    liberdade de integração
        +
    reciprocidade para melhorias
        =
    desenvolvimento aberto sustentável

A EUPL 1.2 atende bem a esse equilíbrio para uma biblioteca modular como o Cosmeron.

## Referências jurídicas e oficiais

[^1]: [European Union Public Licence v1.2 — texto oficial da licença](https://interoperable-europe.ec.europa.eu/licence/european-union-public-licence-version-12-eupl), especialmente artigos 1, 5–8 e 13–15 e o Apêndice.
[^2]: [Introduction to the EUPL](https://interoperable-europe.ec.europa.eu/collection/eupl/introduction-eupl-licence), Interoperable Europe Portal.
[^3]: [Diretiva 2009/24/CE relativa à proteção jurídica dos programas de computador](https://eur-lex.europa.eu/eli/dir/2009/24/oj), especialmente considerandos 10 e 15 e artigo 6.
[^4]: [Matrix of EUPL compatible open source licences](https://interoperable-europe.ec.europa.eu/collection/eupl/matrix-eupl-compatible-open-source-licences), Interoperable Europe Portal.
[^5]: [Textos oficiais da EUPL 1.2 nas 23 línguas da UE](https://interoperable-europe.ec.europa.eu/collection/eupl/eupl-text-eupl-12).
[^6]: [EUPL FAQ — cláusula de exclusão de responsabilidade](https://interoperable-europe.ec.europa.eu/collection/eupl/faqs), Interoperable Europe Portal.
[^7]: [Decisão de Execução (UE) 2017/863 da Comissão](https://eur-lex.europa.eu/eli/dec_impl/2017/863/oj), Jornal Oficial da União Europeia.

Para os termos jurídicos, consulte sempre o texto oficial da EUPL 1.2 e a [LICENSE](../../LICENSE) deste repositório.