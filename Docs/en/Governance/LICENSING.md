# Why EUPL 1.2?

[Português (Brasil)](../Translations/Portugues-Brasil/Governance/LICENSING.md) · [Español (Latinoamérica)](../Translations/Espanol-LATAM/Governance/LICENSING.md)

Cosmeron is licensed under the **European Union Public Licence, Version 1.2 only (EUPL-1.2)**.

This document explains why Cosmeron chose the EUPL. It is informative only and does not modify, replace, or supplement the terms in [LICENSE](../../LICENSE).

## Why EUPL?

### One licence for the project

EUPL 1.2 applies to "the Work", meaning the Original Work or its Derivative Works, and the licence expressly treats that Work as a work or software rather than limiting its scope to executable software alone.[^1]

That makes it suitable for Cosmeron's source code and other copyrightable material distributed as part of the project, such as documentation and technical material. Official EUPL guidance also describes the licence as covering software and ancillary data.[^2]

### Reciprocity without isolating the library

Cosmeron is intended to be useful as a library inside larger projects, including projects distributed under different licensing models.

The EUPL is designed around interoperability. Official EUPL guidance explains that interfaces, APIs and data structures may be reused for implementing static or dynamic linking with independent components without automatically changing the licence of those independent components.[^2]

That guidance relies in particular on **Directive 2009/24/EC on the legal protection of computer programs**. Recital 10 describes interoperability as the ability of software and hardware components to exchange and mutually use information, while Recital 15 and Article 6 establish a limited interoperability exception where reproduction or translation of code is indispensable to obtain the information necessary to make an independently created program interoperate with other programs.[^3]

The EUPL compatibility guidance draws from those provisions when discussing linking. It also notes that there is no specific Court of Justice case law definitively resolving the linking question, so Cosmeron treats this as the official EUPL interpretation and guidance rather than as a judicial holding.[^4]

At the same time, when EUPL-covered modifications or derivative works are distributed or communicated to the public, Article 5 preserves reciprocal obligations for the covered Work and requires source code to be supplied or made freely available.[^1]

In practical terms, the project's goal is:

    Use Cosmeron freely.
    Integrate Cosmeron freely.
    Improve Cosmeron freely.
    Share distributed improvements to Cosmeron.

### Protecting shared improvements

The reciprocal nature of the EUPL helps keep distributed improvements and derivatives of the covered Work available under open terms. Article 5 requires Distribution or Communication of the covered Work or Derivative Works to remain under the EUPL, unless the compatibility mechanism applies.[^1]

For Cosmeron, this supports a shared technical foundation while still allowing the library to participate in larger and differently licensed systems.

### Compatibility

EUPL 1.2 includes an explicit compatibility mechanism in Article 5 for reciprocal free and open-source licences listed in its Appendix.[^1]

The Appendix includes GPL v2/v3, AGPL v3, LGPL v2.1/v3, MPL 2.0, EPL 1.0, CeCILL 2.0/2.1, OSL 2.1/3.0, CC BY-SA 3.0 for non-software works, EUPL 1.1/1.2 and LiLiQ-R/R+.[^1]

The original EUPL-covered Work remains under the EUPL. The compatibility mechanism exists to make distribution of larger combined works possible when applicable licence obligations would otherwise conflict.

### Network use

The EUPL definition of "Distribution or Communication" expressly includes making the Work available online or providing access to its essential functionalities to another person.[^1] Official EUPL guidance therefore describes EUPL 1.2 as covering SaaS and network distribution.[^2]

This means its reciprocity is not limited only to traditional distribution of source-code or executable copies.

### Multilingual by design

EUPL 1.2 has **23 official linguistic versions with equal legal value**. Article 13 expressly provides that linguistic versions approved by the European Commission have identical value and that parties may use the version of their choice.[^1][^5]

That matters to Cosmeron because accessibility should apply not only to source code and documentation, but also to the licence under which the project is shared.

### Chain of authorship

Article 6 contains a chain-of-authorship warranty: the original Licensor warrants that the relevant copyright is owned by or licensed to them and that they have authority to grant the licence; each Contributor makes the same warranty for their modifications.[^1]

Interoperable Europe describes this feature as a **Developer Certificate of Origin (DCO)**.[^2] This should not be confused with a repository workflow requiring a `Signed-off-by:` line on every commit; Cosmeron may adopt such a contribution workflow separately if desired.

### Warranty and liability

The EUPL does not rely on an absolute liability disclaimer. Articles 7 and 8 provide the "as is" warranty disclaimer and a liability exclusion with explicit exceptions for wilful misconduct, damages directly caused to natural persons, and statutory product-liability rules where applicable.[^1]

The official EUPL FAQ explains the policy behind this wording: under European and Member State law, a total exclusion of liability may be invalid, so the EUPL uses a more qualified exclusion intended to operate within applicable mandatory law.[^6]

### European legal foundation

EUPL 1.2 was adopted by **Commission Implementing Decision (EU) 2017/863 of 18 May 2017**. The Decision states that version 1.2 was adopted to further facilitate sharing and reuse, broaden the scope, clarify applicable-law and jurisdiction provisions, and update the compatible-licence appendix.[^7]

The licence itself specifies jurisdiction and applicable-law rules in Articles 14 and 15. For licensors outside the European Union, Article 15 provides for Belgian law unless the parties specifically agree otherwise.[^1]

It may nevertheless be used by projects and developers outside the European Union.

## Our intent

Cosmeron aims to balance three principles:

    freedom to use
        +
    freedom to integrate
        +
    reciprocity for improvements
        =
    sustainable open development

The EUPL 1.2 fits that balance well for a modular library such as Cosmeron.

## Legal and official references

[^1]: [European Union Public Licence v1.2 — official licence text](https://interoperable-europe.ec.europa.eu/licence/european-union-public-licence-version-12-eupl), especially Articles 1, 5–8 and 13–15 and the Appendix.
[^2]: [Introduction to the EUPL](https://interoperable-europe.ec.europa.eu/collection/eupl/introduction-eupl-licence), Interoperable Europe Portal.
[^3]: [Directive 2009/24/EC on the legal protection of computer programs](https://eur-lex.europa.eu/eli/dir/2009/24/oj), especially Recitals 10 and 15 and Article 6.
[^4]: [Matrix of EUPL compatible open source licences](https://interoperable-europe.ec.europa.eu/collection/eupl/matrix-eupl-compatible-open-source-licences), Interoperable Europe Portal.
[^5]: [EUPL 1.2 official texts in 23 EU languages](https://interoperable-europe.ec.europa.eu/collection/eupl/eupl-text-eupl-12).
[^6]: [EUPL FAQ — liability disclaimer](https://interoperable-europe.ec.europa.eu/collection/eupl/faqs), Interoperable Europe Portal.
[^7]: [Commission Implementing Decision (EU) 2017/863](https://eur-lex.europa.eu/eli/dec_impl/2017/863/oj), Official Journal of the European Union.

For legal terms, always refer to the official EUPL 1.2 text and this repository's [LICENSE](../../LICENSE).
